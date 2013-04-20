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

static void
_extract(LRWDict *d, Term *term) {
  if(term->next) term->next->prev = term->prev;
  if(term->prev) term->prev->next = term->next;
  if(term == d->oldest) {
    d->oldest = term->next;
  }
}

static void 
_truncate(LRWDict *d) {
  Term *key = d->oldest;
  if (key) {
    _extract(d, key);
    dictDelete(d->dict, key);
  }
}

static void
_bump(LRWDict *d, Term *term) {
  _extract(d, term);
  
  if(!d->oldest) {
    d->oldest = term;
  }
  
  // insert
  Term *old = d->newest;
  if(old) {
    old->next = term;
  }
  term->prev = old;
  term->next = NULL;
  d->newest = term;
}

void
lrw_dict_put(LRWDict *d, const Term *key, const void *value) {
	dictEntry *entry = dictFind(d->dict, key);
  //   pstring *t = term_to_s(key);
  // printf("adding %.*s to size %d\n", t->len, t->val,dictSize(d->dict));
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
	_bump(d, entry->key);
}

void *
lrw_dict_get(LRWDict *d, const Term *key) {
 	dictEntry *entry = dictFind(d->dict, key);
	if(entry == NULL) {
		return NULL;
	} else {
		return dictGetVal(entry);
	}
}