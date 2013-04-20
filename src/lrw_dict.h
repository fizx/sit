#ifndef LRW_DICT_H_INCLUDED
#define LRW_DICT_H_INCLUDED

#include "dict.h"
#include "ring_buffer.h"

typedef struct lrw_type {
	void *(*next)(void *key);
  void  (*set_next)(void *key, void *next);
} lrw_type;

typedef struct LRWDict {
	dict *dict;
	dictType *dict_type;
	lrw_type *lrw_type;
	long capacity;
	long written;
  void *newest;
  void *oldest;
} LRWDict;

LRWDict *
lrw_dict_new(dictType *dt, lrw_type *lrwt, long capacity);

void
lrw_dict_free(LRWDict *dict);

void *
lrw_dict_get(LRWDict *d, const void *key);

void
lrw_dict_put(LRWDict *d, const void *key, const void *value);

#endif

