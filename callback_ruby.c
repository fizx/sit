#include "sit_ruby.h"

void _cb_mark(void *data) {
	Callback *cb = (Callback *)data;
	rb_gc_mark(vunwrap(cb->user_data));
}

void 
_string_handler(Callback *cb, void *sit_data) {
	VALUE block = vunwrap(cb->user_data);
	pstring *pstr = sit_data;
	VALUE rstr = p2rstring(pstr);
	rb_funcall(block, rb_intern("call"), 1, rstr);
}

void 
_numeric_handler(Callback *cb, void *sit_data) {
  long val = *(long*)sit_data;
  VALUE rval = LONG2NUM(val);
	VALUE block = vunwrap(cb->user_data);
  rb_funcall(block, rb_intern("call"), 1, rval);
}


void 
_query_handler(Callback *cb, void *sit_data) {
	VALUE block = vunwrap(cb->user_data);
	sit_query *query = sit_data;
	if (query == NULL) {
    rb_funcall(block, rb_intern("call"), 1, Qnil);
	} else {
    VALUE class = rb_eval_string("::Sit::Query");
	
  	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, query);
  	rb_obj_call_init(tdata, 0, NULL);
	
  	rb_funcall(block, rb_intern("call"), 1, tdata);
	}
}

void 
_pointer_handler(Callback *cb, void *sit_data) {
	VALUE block = vunwrap(cb->user_data);
  long addr = (long) sit_data;
	rb_funcall(block, rb_intern("call"), 1, LONG2NUM(addr));
}

VALUE
rbc_callback_new(VALUE class, VALUE klass, VALUE block) {
	Callback *cb = callback_new();
	cb->user_data = (void *) vwrap(block);
	
	if (rb_equal(klass, rb_eval_string("::String"))) {
	  cb->handler = _string_handler;
	} else if (rb_equal(klass, rb_eval_string("::Numeric"))) {
	  cb->handler = _numeric_handler;
	} else if (rb_equal(klass, rb_eval_string("::Sit::Query"))) {
	  cb->handler = _query_handler;
  } else {
    cb->handler = _pointer_handler;
  }
	
	VALUE tdata = Data_Wrap_Struct(class, _cb_mark, NULL, cb);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE
rbc_callback_call(VALUE self) {
	Callback *cb;
	Data_Get_Struct(self, Callback, cb);
	
	cb->handler(cb, NULL);
	return Qnil;
}

VALUE
rbc_callback_to_s(VALUE self){
	Callback *cb;
	Data_Get_Struct(self, Callback, cb);
	char *str;
	asprintf(&str, "[Callback %ld]", cb->id);
	VALUE rstr = rb_str_new2(str);
	free(str);
	return rstr;
}
