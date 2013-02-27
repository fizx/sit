#include "sit.h"

LRWDict *
lrw_dict_new(dictType *dt, lrw_type *lrwt, long capacity) {
	LRWDict *dict = malloc(sizeof(LRWDict));
	dict->capacity = capacity;
	dict->dict_type = dt;
	dict->lrw_type = lrwt;
	dict->dict = dictCreate(dt, 0);
	dict->written = 0;
	return dict;
}

void *
lrw_dict_get(LRWDict *d, const void *key) {
	dictEntry *entry = dictFind(d->dict, key);
	if(entry == NULL) {
		return NULL;
	} else {
		return dictGetVal(entry);
	}
}

void
lrw_dict_tap(LRWDict *d, const void *key) {
  assert(key);
  dictEntry *entry = dictFind(d->dict, key);
	d->lrw_type->bump(entry, d->written);
}

void 
_truncate(LRWDict *d) {
	bool success = false;
	dictIterator *iter = dictGetIterator(d->dict);
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

void
lrw_dict_put(LRWDict *d, const void *key, const void *value) {
	dictEntry *entry = dictFind(d->dict, key);
	if(entry == NULL) {
		if(dictSize(d->dict) >= d->capacity) {
			_truncate(d);
		}
		entry = dictAddRaw(d->dict, (void *) key);
		if(!entry && dictIsRehashing(d->dict)) {
			entry = dictFind(d->dict, key);
		}
		assert(entry);
		dictSetVal(d->dict, entry, (void *) value);
		d->lrw_type->bump(entry, d->written++);
	} else {
		dictSetVal(d->dict, entry, (void *) value);
		d->lrw_type->bump(entry, d->written);
	}
}
