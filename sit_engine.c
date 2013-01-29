#include "sit_engine.h"
#include <stdlib.h>
#include <stdio.h>

int query_id = 0;

typedef struct sit_query_node {
	dict                   *children;
	sit_term							 *term;
	struct sit_query_node  *parent;
	sit_callback 				   *callback;
} sit_query_node;

static unsigned int 
_term_hash(const void *key)
{
    return ((sit_term *) key)->hash_code;
}

static int 
_term_compare(void *privdata, const void *key1,
        const void *key2)
{
    DICT_NOTUSED(privdata);

    return _term_hash(key1) == _term_hash(key2);
}

static void
_term_bump(dictEntry *entry, long value) {
	sit_term *term = dictGetKey(entry);
	term->offset = value;
}

static long
_term_version(dictEntry *entry) {
	sit_term *term = dictGetKey(entry);
	return term->offset;
}

dictType termDict = {
    _term_hash,    /* hash function */
    NULL,          /* key dup */
    NULL,          /* val dup */
    _term_compare, /* key compare */
    NULL,    			 /* key destructor */
    NULL           /* val destructor */
};

lrw_type termLrw = {
	_term_bump,
	_term_version
};


sit_engine *
sit_engine_new(sit_parser *parser, long size) {
	int tc = size / 64;
 	sit_engine *engine = malloc(sizeof(sit_engine) + (tc - 1) * (sizeof(sit_term)));
	engine->queries = dictCreate(&termDict, 0);
	engine->parser = parser;
	engine->stream = ring_buffer_new(size / 4);
	engine->stream_append = (void (*)(void *, pstring *)) &ring_buffer_append;
	engine->stream_get =  (pstring* (*)(void *, long, int)) &ring_buffer_get;
	engine->term_dictionary = lrw_dict_new(&termDict, &termLrw, size / 32);
	engine->postings = plist_pool_new(size / 4);
	engine->term_index = dictCreate(&termDict, 0);
	engine->term_count = 0;
	engine->field = NULL;
	engine->term_capacity = tc;
	engine->off = -1;
	engine->len = -1;
	engine->data = NULL;
	return engine;
}

int 
_recurse_add(dict *hash, sit_query_node *parent, sit_term *term, int remaining, sit_callback *callback) {
	if(!term->hash_code) {
		sit_term_update_hash(term);
	}
	sit_query_node *node = dictFetchValue(hash, term);
	if (node == NULL) {
		node = calloc(sizeof(sit_query_node), 1);
		node->parent = parent;
		node->term = term;
		dictAdd(hash, term, node);
	}
	if (remaining == 1) {
		sit_callback *next = node->callback;
		node->callback = callback;
		callback->next = next;
		return (callback->id = query_id++);
	} else {
		if(node->children == NULL) {
			node->children = dictCreate(&termDict, 0);
		}
		return _recurse_add(node->children, node, term + 1, remaining - 1, callback);
	}
}

long
sit_engine_register(sit_engine *engine, sit_query *query) {
	return _recurse_add(engine->queries, NULL, query->terms, query->term_count, query->callback);
}

int
_get_terms(sit_term **terms, sit_query_node *node, int *term_count) {	 
	(*term_count)++;
	if(node->parent) {
		int off = _get_terms(terms, node->parent, term_count);
		terms[off] = node->term;
		return off + 1;
	} else {
		*terms = malloc(sizeof(sit_term*) * (*term_count));
		terms[0] = node->term;
		return 1;
	}
}

void
_recurse_each(dict *hash, sit_callback *cb) {
	dictIterator * iterator = dictGetIterator(hash);
	dictEntry *next;
	while((next = dictNext(iterator))) {
		sit_query_node *node = dictGetVal(next);
		cb->handler(node, cb->user_data);
		if(node->children) {
			_recurse_each(node->children, cb);
		}
	}
}

void
_each_query(void *vnode, void *inner) {
	sit_query_node *node = vnode;
	sit_callback *cb = inner;
	
	sit_callback *qc = node->callback;
	sit_term *terms;
	int term_count = 0;
	while(qc) {
		if (terms == NULL) {
			_get_terms(&terms, node, &term_count);
		}
		sit_query *query = sit_query_new(&terms, term_count, qc);
		cb->handler(query, cb->user_data);
		qc = qc->next;
	}
}

void 
sit_engine_each_query(sit_engine *engine, sit_callback *callback) {
	sit_callback wrapper;
	wrapper.user_data = callback;
	wrapper.handler = _each_query;
	_recurse_each(engine->queries, &wrapper);
}

void 
sit_engine_each_node(sit_engine *engine, sit_callback *callback) {
	_recurse_each(engine->queries, callback);
}

void
sit_engine_percolate(sit_engine *engine, long off, int len) {
	(void) engine;
	(void) off;
	(void) len;
	printf("TODO: sit_engine_percolate\n");
}

void
sit_engine_index(sit_engine *engine, long off, int len) {
	(void) engine;
	(void) off;
	(void) len;
	printf("TODO: sit_engine_index\n");
}

void
_unregister_handler(void *vnode, void *inner) {
	sit_query_node *node = vnode;
	long query_id = *(long *) inner;
	
	while (node->callback && node->callback->id == query_id) {
		sit_callback *old = node->callback;
		node->callback = old->next;
		if(old->free) {
			old->free(old);
		}
	}
	
	sit_callback *prev = node->callback;
	sit_callback *qc = node->callback;
	while(qc) {
		if(qc->id == query_id) {
			prev->next = qc->next;
			if(qc->free) {
				qc->free(qc);
			}
		}
		prev = qc;
		qc = qc->next;
	}
}

void
sit_engine_unregister(sit_engine *engine, long query_id) {
	sit_callback unregister;
	unregister.user_data = &query_id;
	unregister.handler = _unregister_handler;
	_recurse_each(engine->queries, &unregister);
}

void
sit_engine_consume(void *data, pstring *pstr) {
	sit_engine *engine = data;
	engine->stream_append(engine->stream, pstr);
	engine->parser->consume(engine->parser, pstr);
}

void 
sit_engine_term_found(void *data, long off, int len, int field_offset) {
	sit_engine *engine = data;
	sit_term *term = &engine->terms[engine->term_count++];
	term->field = engine->field;
	term->text = engine->stream_get(engine->stream, off, len);
	term->offset = field_offset;
}

void 
sit_engine_document_found(void *data, long off, int len) {
	sit_engine *engine = data;
	sit_engine_percolate(engine, off, len);
	sit_engine_index(engine, off, len);
	engine->term_count = 0;
}

void 
sit_engine_field_found(void *data, pstring *name) {
	sit_engine *engine = data;
	engine->field = name;
}
