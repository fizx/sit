#include "sit_ruby.h"

typedef struct ruby_plus_next {
	void *pointer;
  struct ruby_plus_next *next;
} ruby_plus_next;

static unsigned int
_rb_hash(const void *key) {
	ruby_plus_next *ptr = (ruby_plus_next *)key;
	VALUE rkey = vunwrap(ptr->pointer);
	VALUE rhash = rb_funcall(rkey, rb_intern("hash"), 0);
	unsigned int hash = (unsigned int) NUM2LONG(rhash);
	return hash;
}

VALUE
rbc_lrw_dict_each(VALUE self) {
	LRWDict *d;
	Data_Get_Struct(self, LRWDict, d);	
	dictIterator *iter = dictGetSafeIterator(d->dict);
	if (rb_block_given_p()) {
		dictEntry *entry;
		while ((entry = dictNext(iter)) != NULL) {
			VALUE ary = rb_ary_new();
			ruby_plus_next *ptr = dictGetKey(entry);
			rb_ary_push(ary, vunwrap(ptr->pointer));
			rb_ary_push(ary, vunwrap(dictGetVal(entry)));
	    rb_yield(ary);
		}
  }
	dictReleaseIterator(iter);
	return Qnil;
}

static int 
_rb_compare(void *privdata, const void *key1, const void *key2) {
  DICT_NOTUSED(privdata);
	ruby_plus_next *a = (ruby_plus_next *)key1;
	ruby_plus_next *b = (ruby_plus_next *)key2;
	VALUE ra = vunwrap(a->pointer);
	VALUE rb = vunwrap(b->pointer);
	return (int) rb_equal(ra, rb);
}

static void *
_rb_next(void *key) {
	ruby_plus_next *ptr = key;
  return ptr->next;
}

static void
_rb_set_next(void *key, void *next) {
	ruby_plus_next *ptr = key;
  ptr->next = next;
}

static void *
vvwrap(VALUE key) {
	void *wrapped = (void *)vwrap(key);
	ruby_plus_next *versioned = malloc(sizeof(ruby_plus_next));
	versioned->next = NULL;
	versioned->pointer = wrapped;
	return versioned;
}

lrw_type rubyLRU = {
  _rb_next,
  _rb_set_next,
};

dictType rubyDict = {
  _rb_hash,    /* hash function */
  NULL,          /* key dup */
  NULL,          /* val dup */
  _rb_compare, /* key compare */
  NULL,    			 /* key destructor */
  NULL           /* val destructor */
};

void
_dmark(void *data) {
	LRWDict *d = data;
	dictIterator *iter = dictGetSafeIterator(d->dict);
	dictEntry *entry;
	while ((entry = dictNext(iter)) != NULL) {
		ruby_plus_next *ptr = dictGetKey(entry);
		rb_gc_mark(vunwrap(ptr->pointer));
		rb_gc_mark(vunwrap(dictGetVal(entry)));
	}	
	dictReleaseIterator(iter);
}

VALUE
rbc_lrw_dict_new(VALUE class, VALUE rsize) {
	long size = NUM2LONG(rsize);
	LRWDict *dict = lrw_dict_new(&rubyDict, &rubyLRU, size);
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, dict);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE
rbc_lrw_dict_get(VALUE self, VALUE key) {
	LRWDict *dict;
	Data_Get_Struct(self, LRWDict, dict);
  void *value = lrw_dict_get(dict, vvwrap(key));
	if(value == NULL) {
		return Qnil;
	} else {
		return vunwrap(value);
	}
}

VALUE
rbc_lrw_dict_put(VALUE self, VALUE key, VALUE value) {
	LRWDict *dict;
	Data_Get_Struct(self, LRWDict, dict);
	lrw_dict_put(dict, vvwrap(key), vwrap(value));
	return Qnil;
}

VALUE
rbc_lrw_dict_size(VALUE self) {
	LRWDict *dict;
	Data_Get_Struct(self, LRWDict, dict);	
	return INT2NUM(dictSize(dict->dict));
}

VALUE
rbc_lrw_dict_capacity(VALUE self) {
	LRWDict *dict;
	Data_Get_Struct(self, LRWDict, dict);
	return LONG2NUM(dict->capacity);
}
