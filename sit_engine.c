#include "sit_engine.h"
#include "ring_buffer.h"
#include "dict.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>

int query_id = 0;

typedef struct sit_query_node {
	dict                   *children;
	sit_term							 *term;
	struct sit_query_node  *parent;
	sit_callback 				   *callback;
} sit_query_node;

typedef struct doc_ref {
	long off;
	int len;
} doc_ref;

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

static unsigned int 
_pstr_hash(const void *key) {  
  pstring *pstr = (pstring *) key;
  return pstrhash(pstr);
}

static int 
_pstr_compare(void *privdata, const void *key1,
        const void *key2) {
  (void) privdata;
  pstring *a = (pstring *) key1;
  pstring *b = (pstring *) key2;
  return pstrcmp(a, b) == 0;
}

dictType pstrDict = {
    _pstr_hash,    /* hash function */
    NULL,          /* key dup */
    NULL,          /* val dup */
    _pstr_compare, /* key compare */
    NULL,    			 /* key destructor */
    NULL           /* val destructor */
};

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
	parser->forwards_to = engine;
	engine->stream = ring_buffer_new(size / 4);
	engine->term_dictionary = lrw_dict_new(&termDict, &termLrw, size / 32);
	engine->postings = plist_pool_new(size / 4);
	engine->term_index = dictCreate(&termDict, 0);
	engine->term_count = 0;
	engine->field = c2pstring("default");
	engine->docs = ring_buffer_new((size / 4 / sizeof(doc_ref)) * sizeof(doc_ref));
	engine->term_capacity = tc;
	engine->data = NULL;
	engine->ints_capacity = size / 4;
	engine->ints = dictCreate(&pstrDict, 0);
	return engine;
}

sit_term SENTINEL = {
  NULL,
  NULL,
  0,
  0,
  true
};

int 
_recurse_add(dict *hash, sit_query_node *parent, sit_term *term, int remaining, bool negated_yet, sit_callback *callback) {
	if(!term->hash_code) {
		sit_term_update_hash(term);
	}

	if(term->negated && !negated_yet) {
	  sit_query_node *node = dictFetchValue(hash, &SENTINEL);
	  if (node == NULL) {
  	  node = calloc(sizeof(sit_query_node), 1);
  		node->parent = parent;
  		node->term = &SENTINEL;
  		node->children = dictCreate(&termDict, 0);
      dictAdd(hash, &SENTINEL, node);
		}
    parent = node;
    hash = node->children;
    negated_yet = true;
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
    if(!callback->id) callback->id = ++query_id;
		return callback->id;
	} else {
		if(node->children == NULL) {
			node->children = dictCreate(&termDict, 0);
		}
		return _recurse_add(node->children, node, term + 1, remaining - 1, negated_yet, callback);
	}
}

long
sit_engine_register(sit_engine *engine, sit_query *query) {
  for(int i = 0; i< query->count; i++) {
	  _recurse_add(engine->queries, NULL, query->conjunctions[i]->terms, query->count, false, query->callback);
  }
  return query->callback->id;
}

int
_get_terms(sit_term **terms, sit_query_node *node, int *term_count) {	 
  if(node->term == &SENTINEL){
    if(node->parent) {
      return _get_terms(terms, node->parent, term_count);
    } else {
      *terms = malloc(sizeof(sit_term*) * (*term_count));
  		return 0;
    }
  } else {
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
	sit_term *terms = NULL;
	int term_count = 0;
	while(qc) {
		if (terms == NULL) {
			_get_terms(&terms, node, &term_count);
		}
    conjunction_t *cjs[1];
    cjs[0] = conjunction_new(&terms, term_count);
		sit_query *query = sit_query_new(cjs, 1, qc);
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

pstring *
sit_engine_last_document(sit_engine *engine) {
	long id = sit_engine_last_document_id(engine);
 	pstring *out = sit_engine_get_document(engine, id);
	return out;
}

pstring *
sit_engine_get_document(sit_engine *engine, long doc_id) {
  if (doc_id < 0) {
    return NULL;
  }
  long off = doc_id * sizeof(doc_ref);
  doc_ref *dr = ring_buffer_get(engine->docs, off, sizeof(doc_ref));
  if(dr == NULL) {
    return NULL;
  } else {
		pstring *doc = ring_buffer_get_pstring(engine->stream, dr->off, dr->len);
		return doc;
  }
}

long
sit_engine_last_document_id(sit_engine *engine) {
  return engine->docs->written / sizeof(doc_ref) - 1;
}

void 
callback_recurse(sit_engine *engine, dict *term_index, dict *query_nodes, pstring *doc, bool positive) {
	assert(doc);
  if(positive) {
    sit_query_node *n = dictFetchValue(query_nodes, &SENTINEL);
    if (n) {
      callback_recurse(engine, term_index, n->children, doc, false);
    }
  }
	if(dictSize(term_index) > dictSize(query_nodes) || !positive){
		dictIterator *iterator = dictGetIterator(query_nodes);
		dictEntry *next;
	
		if ((next = dictNext(iterator)) && positive == !!dictFind(term_index, dictGetKey(next))) {
			sit_query_node *node = dictGetVal(next);
			sit_callback *cb = node->callback;
			while(cb) {
				cb->handler(doc, cb->user_data);
				cb = cb->next;
			}
			if(node->children) {
				callback_recurse(engine, term_index, node->children, doc, positive);
			}
		}
	} else {
		dictIterator *iterator = dictGetIterator(term_index);
		dictEntry *next;
		sit_query_node *node;

		if ((next = dictNext(iterator)) && positive == !!(node = dictFetchValue(query_nodes, dictGetKey(next)))) {
			sit_callback *cb = node->callback;
			while(cb) {
				cb->handler(doc, cb->user_data);
				cb = cb->next;
			}
			if(node->children){
				callback_recurse(engine, term_index, node->children, doc,positive);
			}
		}
	}
}

void
sit_engine_percolate(sit_engine *engine) {
	callback_recurse(engine, engine->term_index, engine->queries, sit_engine_last_document(engine), true);
}

void
sit_engine_index(sit_engine *engine, long doc_id) {
	for (int i = 0; i < engine->term_count; i++) {
		sit_term *term = &engine->terms[i];
		plist *value = lrw_dict_get(engine->term_dictionary, term);
		if(value == NULL) {
			value = plist_new(engine->postings);
			lrw_dict_put(engine->term_dictionary, term, value);
		} else {
      lrw_dict_tap(engine->term_dictionary, term);
		}
    plist_entry entry = { doc_id, term->offset };
		plist_append_entry(value, &entry);
	}
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

sit_result_iterator *
sit_engine_search(sit_engine *engine, sit_query *query) {
  sit_result_iterator *iter = malloc(sizeof(sit_result_iterator));
  iter->query = query;
  iter->engine = engine;
  iter->doc_id = LONG_MAX;
  iter->initialized = false;
  iter->cursors = dictCreate(&termDict, 0);
  iter->subs = malloc(sizeof(sub_iterator*) * query->count);
  iter->count = query->count;
  for(int i = 0; i < query->count; i++) {
    conjunction_t *cj = query->conjunctions[i];
    iter->subs[i] = malloc(sizeof(sub_iterator));
    iter->subs[i]->doc_id = LONG_MAX;
    iter->subs[i]->cursors = malloc(sizeof(plist_cursor*) * cj->count);
    iter->subs[i]->negateds = malloc(cj->count);
    iter->subs[i]->state = malloc(sizeof(long) * cj->count);
    iter->subs[i]->initialized = false;
    iter->subs[i]->count = cj->count;
    for(int j = 0; j < cj->count; j++) {
      sit_term *term = &cj->terms[j];
      plist_cursor *cursor = dictFetchValue(iter->cursors, term);
      if(cursor == NULL) {
        plist *pl = lrw_dict_get(engine->term_dictionary, term);
        cursor = pl == NULL ? NULL : plist_cursor_new(pl);
        dictAdd(iter->cursors, term, cursor);
      }
      if(term->negated) {
        iter->subs[i]->negateds[j] = 1;
      }
      iter->subs[i]->cursors[j] = cursor;
      iter->subs[i]->state[j] = LONG_MAX;
    }
  }
  return iter; 
}

// TODO: not that efficient
bool
sit_result_sub_iterator_prev(sub_iterator *iter) {
  int size = iter->count;
  long min = iter->doc_id;
  min--;
  while (min >= 0) {
    long max = -1;
    for (int i = 0; i < size; i++) {
      plist_cursor *cursor = iter->cursors[i];
      int negated = iter->negateds[i];

      if (cursor == NULL) {
        if(negated) {
          continue;
        } else {
          iter->doc_id = -1;
          return false;
        }
      }

      if(!iter->initialized) {
        iter->state[i] = LONG_MAX;
        plist_cursor_prev(cursor);
      }
      
      if (negated) {
        long lower;
        while((lower = plist_cursor_document_id(cursor)) >= min || iter->state[i] >= min) {
          iter->state[i] = lower;
          if(!plist_cursor_prev(cursor)) {
            lower = -1;
            break;
          }
        }  

        min = lower - 1;
        if(min > max) max = min;
      } else {
        
        long doc;
        while((doc = plist_cursor_document_id(cursor)) > min) {
          plist_cursor_prev(cursor);
        }
        if(doc < min) min = doc;
        if(doc > max) max = doc;
      }
    }
    if(max == -1) {
      iter->doc_id = -1;
      return false;
    }
    iter->initialized = true;
    if(min == max) break;
  }
  
  iter->doc_id = min;
  return min >= 0;
}

// TODO: not that efficient
bool
sit_result_iterator_prev(sit_result_iterator *iter) {
  int size = iter->count;

  long bound = iter->doc_id;
  bound--;
  long max = -1;
  for (int i = 0; i < size; i++) {
    sub_iterator *subiter = iter->subs[i];
    if (subiter == NULL) {
      iter->doc_id = -1;
      return false;
    }

    if(!iter->initialized) {
      sit_result_sub_iterator_prev(subiter);
    }
    
    while(subiter->doc_id > bound) {
      sit_result_sub_iterator_prev(subiter);
    }
    
    if(subiter->doc_id > max) max = subiter->doc_id;
  }

  iter->initialized = true;
  iter->doc_id = max;
  
  return max >= (long)0;
}

void
sit_result_iterator_do_callback(sit_result_iterator *iter) {
  iter->query->callback->handler(sit_result_iterator_document(iter), iter->query->callback->user_data);
}


pstring *
sit_result_iterator_document(sit_result_iterator *iter) {
  return sit_engine_get_document(iter->engine, sit_result_iterator_document_id(iter));
}

long
sit_result_iterator_document_id(sit_result_iterator *iter) {
  return iter->doc_id;
}

void
sit_engine_unregister(sit_engine *engine, long query_id) {
	sit_callback unregister;
	unregister.user_data = &query_id;
	unregister.handler = _unregister_handler;
	_recurse_each(engine->queries, &unregister);
}

void
sit_engine_consume(sit_engine *engine, pstring *pstr) {
  ring_buffer_append_pstring(engine->stream, pstr);
	engine->parser->consume(engine->parser, pstr);
}

void 
sit_engine_term_found(sit_engine *engine, long off, int len, int field_offset) {
	sit_term *term = &engine->terms[engine->term_count++];
	term->field = engine->field;
	term->text = ring_buffer_get_pstring(engine->stream, off, len);
	term->offset = field_offset;
	sit_term_update_hash(term);
	dictAdd(engine->term_index, term, term);
}

void
sit_engine_zero_ints(sit_engine *engine) {  
  pstring *orig_field = engine->field;
  dictIterator *iter = dictGetIterator(engine->ints);
  dictEntry *entry;
  while ((entry = dictNext(iter))) {
    engine->field = dictGetKey(entry);
    sit_engine_int_found(engine, 0);
  }
  engine->field = orig_field;
  dictReleaseIterator(iter);
}

void 
sit_engine_document_found(sit_engine *engine, long off, int len) {
	assert(off >= 0);
	assert(len > 0);
  doc_ref dr = { off, len };
	ring_buffer_append(engine->docs, &dr, sizeof(dr));
  long doc_id = sit_engine_last_document_id(engine);
	sit_engine_percolate(engine);
	sit_engine_index(engine, doc_id);
  sit_engine_zero_ints(engine);
	engine->term_count = 0;
	dictEmpty(engine->term_index);
}

int *
sit_engine_get_int(sit_engine *engine, long doc_id, pstring *field) {
  ring_buffer *rb = dictFetchValue(engine->ints, field);
  long off = doc_id * sizeof(int);
  if(rb == NULL ||
    (off + sizeof(int) > rb->written) ||
    (off < rb->written - rb->capacity)
    ) {
    return NULL;
  } else {
    return (int *) &rb->buffer[off % rb->capacity];
  }
}

void
sit_engine_set_int(sit_engine *engine, long doc_id, pstring *field, int value) {
  ring_buffer *rb = dictFetchValue(engine->ints, field);
  assert(field);
  if(rb == NULL) {
    rb = ring_buffer_new(engine->ints_capacity);
    dictAdd(engine->ints, engine->field, rb);
  }
  ring_buffer_put(rb, doc_id * sizeof(int), &value, sizeof(int));
}

void
sit_engine_incr(sit_engine *engine, long doc_id, pstring *field, int value) {
  int *ptr = sit_engine_get_int(engine, doc_id, field);
  if(ptr != NULL) {
    *ptr += value;
  }
}

void 
sit_engine_int_found(sit_engine *engine, int value) {
  long doc_id = sit_engine_last_document_id(engine) + 1;
  sit_engine_set_int(engine, doc_id, engine->field, value);
}

void 
sit_engine_field_found(sit_engine *engine, pstring *name) {
	engine->field = name;
}
