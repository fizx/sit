#ifndef PLIST_H_INCLUDED
#define PLIST_H_INCLUDED

#include "sit.h"
#include "cursor.h"
#include <stdbool.h>

typedef struct PlistEntry {
	int doc;
	int pos;
} PlistEntry;

typedef struct PlistBlock {
	struct PlistBlock *prev;
	struct PlistBlock *next;
 	int size;
	int prev_version;
	int entries_count;
	PlistEntry entries[];
} PlistBlock;

typedef struct Plist {
	struct PlistPool  *pool;
	PlistBlock *last_block;
	int last_version;
} Plist;

typedef struct free_list {
	struct free_list *next;
} free_list;

typedef struct PlistPool {
	void         *buffer;
	long          capacity;
	int           current_version;
	int           min_version;
	long          region_size;
	int          	region_count;
	int 					default_block_size;
	Plist        *lowest_plist;
  free_list    *free_list;
 	void         *next_block;
} PlistPool;

typedef struct plist_cursor {
  struct Cursor as_cursor;
  Plist *plist;
  PlistBlock *block;
  bool exhausted;
} plist_cursor;

void
plist_pool_free(PlistPool *plist);

PlistPool *
plist_pool_new(long size);

plist_cursor *
plist_cursor_new(Plist *plist);

Plist *
plist_new(PlistPool *pool);

long
plist_size(Plist *plist);

void
plist_free(Plist *plist);

void
plist_each(Plist *plist, struct Callback *handler);

void
plist_reach(Plist *plist, struct Callback *handler);

void
plist_append_entry(Plist *pl, PlistEntry *entry);

#endif
