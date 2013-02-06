#ifndef PLIST_H_INCLUDED
#define PLIST_H_INCLUDED

#include "sit_callback.h"
#include <stdbool.h>

struct plist_pool;

typedef struct {
	int doc;
	int pos;
} plist_entry;

typedef struct _plist_block {
	struct _plist_block *prev;
	struct _plist_block *next;
 	int size;
	int prev_version;
	int entries_count;
	plist_entry entries[];
} plist_block;

typedef struct {
	struct plist_pool  *pool;
	plist_block *last_block;
	int last_version;
} plist;

typedef struct _free_list {
	struct _free_list *next;
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

typedef struct {
  plist *plist;
  plist_block *block;
  plist_entry *entry;
  bool exhausted;
} plist_cursor;

plist_pool *
plist_pool_new(long size);

plist_cursor *
plist_cursor_new(plist *plist);

long 
plist_cursor_seek_lte(plist_cursor *cursor, long value);

plist *
plist_new(plist_pool *pool);

long
plist_size(plist *plist);

void
plist_free(plist *plist);

void
plist_each(plist *plist, sit_callback *handler);

void
plist_reach(plist *plist, sit_callback *handler);

void
plist_append_entry(plist *pl, plist_entry *entry);

bool
plist_cursor_prev(plist_cursor *cursor);

bool
plist_cursor_next(plist_cursor *cursor);

plist_entry *
plist_cursor_entry(plist_cursor *cursor);

long
plist_cursor_document_id(plist_cursor *cursor);

#endif
