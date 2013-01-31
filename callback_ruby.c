#include "sit_callback.h"
#include "callback_ruby.h"
#include "pstring_ruby.h"
#include "util_ruby.h"
#include <assert.h>

void _cb_mark(void *data) {
	sit_callback *cb = (sit_callback *)data;
	rb_gc_mark(vunwrap(cb->user_data));
}

void 
_handler(void *sit_data, void *user_data) {
	VALUE block = vunwrap(user_data);
	pstring *pstr = sit_data;
	VALUE rstr = p2rstring(pstr);
	rb_funcall(block, rb_intern("call"), 1, rstr);
}

void 
_free(sit_callback *cb) {
	if(cb->next != NULL) {
		cb->next->free(cb->next);
	}
	free(cb);
}

VALUE
rbc_callback_new(VALUE class, VALUE rstr, VALUE block) {
	sit_callback *cb = sit_callback_new();
	cb->user_data = vwrap(block);
	cb->handler = _handler;
	VALUE tdata = Data_Wrap_Struct(class, _cb_mark, _free, cb);
	rb_obj_call_init(tdata, 0, NULL);
	rb_iv_set(tdata, "@rstr", StringValue(rstr));
	return tdata;
}

VALUE
rbc_callback_call(VALUE self) {
	sit_callback *cb;
	Data_Get_Struct(self, sit_callback, cb);
	
	VALUE rstr = rb_iv_get(self, "@rstr");
	
		
	cb->handler(r2pstring(rstr), cb->user_data);
	return Qnil;
}

VALUE
rbc_callback_to_s(VALUE self){
	sit_callback *cb;
	Data_Get_Struct(self, sit_callback, cb);
	char *str;
	asprintf(&str, "[Callback %ld]", cb->id);
	VALUE rstr = rb_str_new2(str);
	free(str);
	return rstr;
}
