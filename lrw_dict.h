#ifndef LRW_DICT_H_INCLUDED
#define LRW_DICT_H_INCLUDED

#include "dict.h"
#include "ring_buffer.h"

typedef struct lrw_type {
	void (*bump)(dictEntry *entry, long value);
	long (*version)(dictEntry *entry);
} lrw_type;

typedef struct {
	dict *dict;
	dictType *dict_type;
	lrw_type *lrw_type;
	long capacity;
	long written;
} lrw_dict;

lrw_dict *
lrw_dict_new(dictType *dt, lrw_type *lrwt, long capacity);

void *
lrw_dict_get(lrw_dict *d, const void *key);

void
lrw_dict_put(lrw_dict *d, const void *key, const void *value);

#endif

