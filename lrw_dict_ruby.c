#include "lrw_dict_ruby.h"
#include "util_ruby.h"
#include "lrw_dict.h"

typedef struct {
	void *pointer;
	long version;
} versioned_ruby_pointer;

static unsigned int
_rb_hash(const void *key) {
	versioned_ruby_pointer *ptr = (versioned_ruby_pointer *)key;
	VALUE rkey = vunwrap(ptr->pointer);
	VALUE rhash = rb_funcall(rkey, rb_intern("hash"), 0);
	unsigned int hash = (unsigned int) NUM2LONG(rhash);
	return hash;
}

VALUE
rbc_lrw_dict_each(VALUE self) {
	lrw_dict *d;
	Data_Get_Struct(self, lrw_dict, d);	
	dictIterator *iter = dictGetSafeIterator(d->dict);
	if (rb_block_given_p()) {
		dictEntry *entry;
		while ((entry = dictNext(iter)) != NULL) {
			VALUE ary = rb_ary_new();
			versioned_ruby_pointer *ptr = dictGetKey(entry);
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
	versioned_ruby_pointer *a = (versioned_ruby_pointer *)key1;
	versioned_ruby_pointer *b = (versioned_ruby_pointer *)key2;
	VALUE ra = vunwrap(a->pointer);
	VALUE rb = vunwrap(b->pointer);
	return (int) rb_equal(ra, rb);
}

static void
_rb_bump(dictEntry *entry, long version) {
	versioned_ruby_pointer *ptr = dictGetKey(entry);
	ptr->version = version;
}

static long
_rb_version(dictEntry *entry) {
	versioned_ruby_pointer *ptr = dictGetKey(entry);
	return ptr->version;
}
static void *
vvwrap(VALUE key) {
	void *wrapped = (void *)vwrap(key);
	versioned_ruby_pointer *versioned = malloc(sizeof(versioned_ruby_pointer));
	versioned->version = 0;
	versioned->pointer = wrapped;
	return versioned;
}

lrw_type rubyLRU = {
  _rb_bump,  
	_rb_version
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
	lrw_dict *d = data;
	dictIterator *iter = dictGetSafeIterator(d->dict);
	dictEntry *entry;
	while ((entry = dictNext(iter)) != NULL) {
		versioned_ruby_pointer *ptr = dictGetKey(entry);
		rb_gc_mark(vunwrap(ptr->pointer));
		rb_gc_mark(vunwrap(dictGetVal(entry)));
	}	
	dictReleaseIterator(iter);
}

VALUE
rbc_lrw_dict_new(VALUE class, VALUE rsize) {
	long size = NUM2LONG(rsize);
	lrw_dict *dict = lrw_dict_new(&rubyDict, &rubyLRU, size);
	VALUE tdata = Data_Wrap_Struct(class, _dmark, NULL, dict);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE
rbc_lrw_dict_get(VALUE self, VALUE key) {
	lrw_dict *dict;
	Data_Get_Struct(self, lrw_dict, dict);
	const void *value = lrw_dict_get(dict, vvwrap(key));
	if(value == NULL) {
		return Qnil;
	} else {
		return vunwrap(value);
	}
}

VALUE
rbc_lrw_dict_put(VALUE self, VALUE key, VALUE value) {
	lrw_dict *dict;
	Data_Get_Struct(self, lrw_dict, dict);
	lrw_dict_put(dict, vvwrap(key), vwrap(value));
	return Qnil;
}

VALUE
rbc_lrw_dict_size(VALUE self) {
	lrw_dict *dict;
	Data_Get_Struct(self, lrw_dict, dict);	
	return INT2NUM(dictSize(dict->dict));
}

VALUE
rbc_lrw_dict_capacity(VALUE self) {
	lrw_dict *dict;
	Data_Get_Struct(self, lrw_dict, dict);
	return LONG2NUM(dict->capacity);
}
