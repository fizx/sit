// typedef struct {
// 	long (*hash_function)(void *key);
// 	long capacity;
// 	dict *dict;
//  	long **ring;
// } lrw_dict;

#include "lrw_dict.h"
#include <stdlib.h>
#include <stdbool.h>

lrw_dict *
lrw_dict_new(dictType *dt, lrw_type *lrwt, long capacity) {
	lrw_dict *dict = malloc(sizeof(lrw_dict));
	dict->capacity = capacity;
	dict->dict_type = dt;
	dict->lrw_type = lrwt;
	dict->dict = dictCreate(dt, 0);
	dict->written = 0;
	return dict;
}

void *
lrw_dict_get(lrw_dict *d, const void *key) {
	dictEntry *entry = dictFind(d->dict, key);
	if(entry == NULL) {
		return NULL;
	} else {
		return dictGetVal(entry);
	}
}

void
lrw_dict_put(lrw_dict *d, const void *key, const void *value) {
	dictEntry *entry = dictFind(d->dict, key);
	if(entry == NULL) {
		if(dictSize(d->dict) >= d->capacity) {
			bool success = false;
			dictIterator *iter = dictGetSafeIterator(d->dict);
			dictEntry *entry;
			dictEntry *min;
			long min_version = d->written;
			
			while ((entry = dictNext(iter)) != NULL) {
				long version = d->lrw_type->version(entry);
				if(version < min_version) {
					min = entry;
					min_version = version;
				}
				
				if(version <= d->written - d->capacity) {
					dictDelete(d->dict, dictGetKey(entry));
					success = true;
					break;
				}
			}	
			
			// TODO: replace with something more efficient.  We start
			// hitting this when we O(n) loop on duplicate keys.  We can
			// count/track the duplications and hit that break very early, which
			// will make this O(1) amortized?!
			if(!success) {
				dictDelete(d->dict, dictGetKey(min));
			}
			
			dictReleaseIterator(iter);
		}
		entry = dictAddRaw(d->dict, (void *) key);
		dictSetVal(d->dict, entry, (void *) value);
		d->lrw_type->bump(entry, d->written++);
	} else {
		dictSetVal(d->dict, entry, (void *) value);
		d->lrw_type->bump(entry, d->written);
	}
}
