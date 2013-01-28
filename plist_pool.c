#include "plist_pool.h"
#include <stdlib.h>

plist_pool *
plist_pool_new(long size) {
	plist_pool *pool = malloc(sizeof(plist_pool));
	pool->capacity = size;
	pool->buffer = malloc(size);
	
	return pool;
}
