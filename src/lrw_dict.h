#ifndef LRW_DICT_H_INCLUDED
#define LRW_DICT_H_INCLUDED

#include "dict.h"
#include "ring_buffer.h"

typedef struct LRWDict {
	dict *dict;
	dictType *dict_type;
	long capacity;
	long written;
  Term *newest;
  Term *oldest;
} LRWDict;

LRWDict *
lrw_dict_new(dictType *dt, long capacity);

void
lrw_dict_free(LRWDict *dict);

void *
lrw_dict_get(LRWDict *d, const Term *key);

void
lrw_dict_put(LRWDict *d, const Term *key, const void *value);

#endif

