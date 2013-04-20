#include "sit_ruby.h"

VALUE
rbc_lrw_dict_each(VALUE self) {
	LRWDict *d;
	Data_Get_Struct(self, LRWDict, d);	
  
	if (rb_block_given_p()) {
	  Term *k = d->oldest; 
    assert(!d->oldest || !d->oldest->prev);
	  while(k) {
	    VALUE ary = rb_ary_new();
			dictEntry *entry = dictFind(d->dict, k);
			VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::Term"), markall, NULL, k);
    	rb_obj_call_init(tdata, 0, NULL);
			rb_ary_push(ary, tdata);
			rb_ary_push(ary, vunwrap(dictGetVal(entry)));
      rb_yield(ary);
      k = k->next;
	  }
  }
	return Qnil;
}

VALUE
rbc_lrw_dict_new(VALUE class, VALUE rsize) {
	long size = NUM2LONG(rsize);
	LRWDict *dict = lrw_dict_new(getTermPlistDict(), size);
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, dict);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE
rbc_lrw_dict_get(VALUE self, VALUE key) {
	LRWDict *dict;
  Term *term;
	Data_Get_Struct(key, Term, term);
	Data_Get_Struct(self, LRWDict, dict);
  void *value = lrw_dict_get(dict, term);
	if(value == NULL) {
		return Qnil;
	} else {
		return vunwrap(value);
	}
}

VALUE
rbc_lrw_dict_put(VALUE self, VALUE key, VALUE value) {
	LRWDict *dict;
  Term *term;
	Data_Get_Struct(key, Term, term);
	Data_Get_Struct(self, LRWDict, dict);
	lrw_dict_put(dict, term, vwrap(value));
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
