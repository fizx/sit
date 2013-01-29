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
 	sit_engine *engine = malloc(sizeof(sit_engine));
	engine->queries = dictCreate(&termDict, 0);
	engine->parser = parser;
	engine->stream = ring_buffer_new(size / 4);
	engine->stream_append = (void (*)(void *, pstring *)) ring_buffer_append;
	engine->stream_get =  (void (*)(void *, long, int)) ring_buffer_get;
	engine->term_dictionary = lrw_dict_new(&termDict, &termLrw, size / 32);
	engine->postings = plist_pool_new(size / 4);
	engine->term_index = dictCreate(&termDict, 0);
	engine->term_count = 0;
	engine->field = NULL;
	engine->term_capacity = size / 64;
	engine->off = -1;
	engine->len = -1;
	engine->terms = malloc(sizeof(sit_term) * engine->term_capacity);
	engine->data = NULL;
	return engine;
}