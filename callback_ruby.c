#include "sit_callback.h"
#include "callback_ruby.h"

void _cb_mark(void *data) {
	sit_callback *cb = (sit_callback *)data;
	rb_gc_mark((VALUE)(cb->user_data));
}

void 
_handler(void *sit_data, void *user_data) {
	VALUE a = (VALUE) user_data;
	VALUE block = rb_ary_pop(a);
	VALUE ruser_data = rb_ary_pop(a);
	rb_funcall(block, rb_intern("call"), 2, (VALUE) sit_data, ruser_data);
}

void 
_free(sit_callback *cb) {
	if(cb->next != NULL) {
		cb->next->free(cb->next);
	}
	free(cb);
}

VALUE
rbc_callback_new(VALUE class, VALUE user_data, VALUE block) {
	VALUE values = rb_ary_new();
	rb_ary_push(values, user_data);
	rb_ary_push(values, block);
	sit_callback *cb = sit_callback_new();
	cb->user_data = (void *) values;
	cb->handler = _handler;
	VALUE tdata = Data_Wrap_Struct(class, _cb_mark, _free, cb);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
	
}

VALUE
rbc_callback_call(VALUE self, VALUE rdata) {
	sit_callback *cb;
	Data_Get_Struct(self, sit_callback, cb);
	cb->handler((void*) rdata, cb->user_data);
	return Qnil;
}

VALUE
rbc_callback_to_s(VALUE self){
	sit_callback *cb;
	Data_Get_Struct(self, sit_callback, cb);
	char *str;
	asprintf(&str, "[Callback %d]", cb->id);
	VALUE rstr = rb_str_new2(str);
	free(str);
	return rstr;
}
