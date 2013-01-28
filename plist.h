#ifndef PLIST_H_INCLUDED
#define PLIST_H_INCLUDED

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
	int 					default_block_size;
	plist        *lowest_plist;
  free_list    *free_list;
 	void         *next_block;
} plist_pool;

typedef struct {
	void *data;
	void (*handle)(void *data, plist_entry *entry);
} plist_iterator;

plist_pool *
plist_pool_new(long size);

plist *
plist_new(plist_pool *pool);

long
plist_size(plist *plist);

void
plist_free(plist *plist);

void
plist_each(plist *plist, plist_iterator *handler);

void
plist_reach(plist *plist, plist_iterator *handler);

void
plist_append_entry(plist *pl, plist_entry *entry);

#endif
