#include "sit.h"

LRWDict *
lrw_dict_new(dictType *dt, lrw_type *lrwt, long capacity) {
	LRWDict *dict = calloc(1, sizeof(LRWDict));
	dict->capacity = capacity;
	dict->dict_type = dt;
	dict->lrw_type = lrwt;
	dict->dict = dictCreate(dt, 0);
	dict->written = 0;
	
	return dict;
}

void
lrw_dict_free(LRWDict *dict) {
  dictRelease(dict->dict);
  free(dict);
}

void 
_truncate(LRWDict *d) {
  void *key = d->oldest;
  if(!key) return;
  
  void *next = d->lrw_type->next(key);
  dictDelete(d->dict, key);
  d->oldest = next;
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
	}
	dictSetVal(d->dict, entry, (void *) value);
  void *newest = d->newest;
  if(newest) {
    d->lrw_type->set_next(newest, entry->key);
  }
  d->newest = entry->key;
  if(!d->oldest) {
    d->oldest = entry->key;
  }
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