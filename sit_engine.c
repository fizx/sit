#include "sit_engine.h"
#include <stdlib.h>
#include <stdio.h>

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
	printf("TODO: FOUND DOC IS NO-OP\n");
}

void 
sit_engine_field_found(void *data, pstring *name) {
	sit_engine *engine = data;
	engine->field = name;
}
