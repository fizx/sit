#ifndef PLIST_POOL_H_INCLUDED
#define PLIST_POOL_H_INCLUDED

typedef struct {
	char **buffer;
	long capacity;
	int min_version;
} plist_pool;

typedef struct {
	int doc;
	int pos;
} plist_entry;

typedef struct _plist_block {
	struct _plist_block *prev;
	struct _plist_block *next;
	int prev_version;
	int entries_count;
	plist_entry entries[];
} plist_block;

typedef struct {
	int block_count;
	plist_block *last;
	int last_version;
} plist;

plist_pool *
plist_pool_new(long size);

#endif

