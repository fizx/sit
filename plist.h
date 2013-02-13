#ifndef PLIST_H_INCLUDED
#define PLIST_H_INCLUDED

#include "sit.h"
#include "sit_cursor.h"
#include <stdbool.h>

typedef struct plist_entry {
	int doc;
	int pos;
} plist_entry;

typedef struct plist_block {
	struct plist_block *prev;
	struct plist_block *next;
 	int size;
	int prev_version;
	int entries_count;
	plist_entry entries[];
} plist_block;

typedef struct plist {
	struct plist_pool  *pool;
	plist_block *last_block;
	int last_version;
} plist;

typedef struct free_list {
	struct free_list *next;
} free_list;

typedef struct plist_pool {
	void         *buffer;
	long          capacity;
	int           current_version;
	int           min_version;
	long          region_size;
	int          	region_count;
	int 					default_block_size;
	plist        *lowest_plist;
  free_list    *free_list;
 	void         *next_block;
} plist_pool;

typedef struct plist_cursor {
  struct sit_cursor as_cursor;
  plist *plist;
  plist_block *block;
  bool exhausted;
} plist_cursor;

plist_pool *
plist_pool_new(long size);

plist_cursor *
plist_cursor_new(plist *plist);

plist *
plist_new(plist_pool *pool);

long
plist_size(plist *plist);

void
plist_free(plist *plist);

void
plist_each(plist *plist, struct sit_callback *handler);

void
plist_reach(plist *plist, struct sit_callback *handler);

void
plist_append_entry(plist *pl, plist_entry *entry);

#endif
