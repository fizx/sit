#include "sit.h"

LRWDict *
lrw_dict_new(dictType *dt, long capacity) {
	LRWDict *dict = calloc(1, sizeof(LRWDict));
	dict->capacity = capacity;
	dict->dict_type = dt;
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
  Term *key = d->oldest;
  if(!key) return;

  Term *next  = ((Term *)key)->next;
  next->prev = NULL;
  d->oldest = next;
  dictDelete(d->dict, key);
}

static void
_set_next(Term *ptr, Term *next) {
  
  if(next->next) next->next->prev = next->prev;
  if(next->prev) next->prev->next = next->next;
  
  next->prev = ptr;
  ptr->next = next;
  next->next = NULL;
	
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
  if(newest == entry->key) {
    return;
  }
  if(newest) {
    _set_next(newest, entry->key);
  }
  if(entry->key == d->oldest) {
    d->oldest = ((Term*)entry->key)->next;
  }
  d->newest = entry->key;
  if(!(d->oldest)) {
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