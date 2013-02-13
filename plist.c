#include "plist.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>

plist_pool *
plist_pool_new(long size) {
	plist_pool *pool = malloc(sizeof(plist_pool));
	pool->capacity = size;
	pool->buffer = malloc(size);
	pool->next_block = (plist_block *)pool->buffer;
	pool->default_block_size = 512; // bytes
	pool->region_size = size / 16;
	pool->region_count = 16;
	pool->current_version = 0;
	pool->min_version = 0;
	return pool;
}

long
plist_cursor_document_id(sit_cursor *scursor) {
  plist_cursor *cursor = (plist_cursor *)scursor;
  if(cursor->as_cursor.data != NULL) {
    return ((plist_entry *)cursor->as_cursor.data)->doc;
  } else {
    return -1;
  }
}

bool
plist_cursor_prev(sit_cursor *scursor) {
  plist_cursor *cursor = (plist_cursor *)scursor;
  if(cursor->exhausted) {
    return false;
  }
  plist *pl = cursor->plist;
  plist_pool *pool = pl->pool;
  
  if(cursor->block == NULL) {
    if(pl->last_block && pl->last_version >= pool->min_version) {
      cursor->block = pl->last_block;
    } else {
      cursor->exhausted = true;
      return false;
    }
  }
  
  if(cursor->as_cursor.data == NULL) {
    int size = cursor->block->entries_count;
    if (size == 0) {
      cursor->exhausted = true;
      return false;
    } else {
      cursor->as_cursor.data = &cursor->block->entries[size - 1];
      return true;
    }
  } else if (cursor->as_cursor.data == &cursor->block->entries[0]) {
    if(cursor->block->prev && cursor->block->prev_version >= pool->min_version) {
      cursor->block = cursor->block->prev;
      int size = cursor->block->entries_count;
      if (size == 0) {
        cursor->exhausted = true;
        return false;
      } else {
        cursor->as_cursor.data = &cursor->block->entries[size - 1];
        return true;
      }      
    } else {
      cursor->exhausted = true;
      return false;
    }
  } else {
    plist_entry * entry = cursor->as_cursor.data;
    entry--;
    return true;
  }
}

bool
plist_cursor_next(sit_cursor *scursor) {
  plist_cursor *cursor = (plist_cursor *)scursor;
  (void) cursor;
  return false;
}

plist_entry *
plist_cursor_entry(sit_cursor *scursor) {
  plist_cursor *cursor = (plist_cursor *)scursor;
  return cursor->exhausted ? NULL : cursor->as_cursor.data;
}


long 
plist_cursor_seek_lte(sit_cursor *scursor, long value) {
  plist_cursor *cursor = (plist_cursor *)scursor;
	long doc;
	while((doc = plist_cursor_document_id(&cursor->as_cursor)) > value) {
		if(!plist_cursor_prev(&cursor->as_cursor)) {
			doc = -1;
			break;
		}
	}	
	return doc;
}


plist *
plist_new(plist_pool *pool) {
	assert(pool);
	plist *pl = malloc(sizeof(plist));
	pl->pool = pool;
	pl->last_block = NULL;
	pl->last_version = INT_MIN;
	return pl;
}

void 
plist_free(plist *pl) {
  (void) pl;
	// free(pl);
}

plist_cursor *
plist_cursor_new(plist *pl) {
  assert(pl);
  plist_cursor *cursor = malloc(sizeof(plist_cursor));
  cursor->as_cursor.prev = plist_cursor_prev;
  cursor->as_cursor.next = plist_cursor_next;
  cursor->as_cursor.id = plist_cursor_document_id;
  cursor->as_cursor.seek_lte = plist_cursor_seek_lte;
  cursor->as_cursor.data = NULL;
  cursor->plist = pl;
  cursor->block = NULL;
  cursor->exhausted = false;
  return cursor;
}

plist_block *
plist_append_block(plist *pl) {
	plist_pool *pool = pl->pool;
	char *next_block = pool->next_block;
	int current_region = pool->current_version % pool->region_count;
	char *region_cutoff = ((char*)pool->buffer) + current_region * pool->region_size;

	int size = pool->default_block_size;
	if(pl->last_block && pl->last_version >= pool->min_version) {
		size = pl->last_block->size;
		if(pl->last_version == pool->current_version && size * 2 < pool->region_size) {
			size *= 2;
		}
	}
	
	if(next_block + size > region_cutoff) {
		pool->current_version++;
		pool->min_version = pool->current_version - pool->region_count + 1;
		next_block = ((char*)pool->buffer) + current_region * pool->region_size;
	}
	
	plist_block *block = (void *) next_block;
	block->prev = pl->last_block;
	block->prev_version = pl->last_version;
	block->entries_count = 0;
	block->next = NULL;
	if(block->prev && block->prev_version >= pl->pool->min_version) {
		block->prev->next = block;
	}
	block->size = size;
	pl->pool->next_block = ((char *) block) + block->size;
	pl->last_block = block;
	pl->last_version = pl->pool->current_version;
	return block;
}

bool
plist_block_is_full(plist_block *block) {
	long size = block->size;
	char *base = (char *) block;
	char *next = (char *) &block->entries[block->entries_count + 1];
	return next - base > size;
}

void
plist_append_entry(plist *pl, plist_entry *entry) {
	plist_block *block;
	if(pl->last_block == NULL || 
	   pl->last_version < pl->pool->current_version ||
	   plist_block_is_full(pl->last_block)) {
		block = plist_append_block(pl);
	} else {
		block = pl->last_block;
	}
	memcpy(&block->entries[block->entries_count++], entry, sizeof(plist_entry));
}

void
_count(sit_callback *cb, void *entry) {
	(*(int *)cb->user_data)++;
	(void) entry;
}

long
plist_size(plist *plist) {
	sit_callback counter;
	int count = 0;
	counter.handler = _count;
	counter.user_data = &count;
	plist_reach(plist, &counter);
	return count;
}



void
plist_each(plist *pl, sit_callback *iterator) {
	//TODO: impl
	(void) pl;
	(void) iterator;
}

void
plist_reach(plist *pl, sit_callback *iterator) {
	int min = pl->pool->min_version;
	if(pl->last_version >= min) {
		plist_block *block = pl->last_block;
		while(block) {
			for (int i = block->entries_count - 1; i >= 0; i--) {
				iterator->handler(iterator, &block->entries[i]);
			}
			if(block->prev_version >= min) {
				block = block->prev;
			} else {
				break;
			}
		}
	}
}
