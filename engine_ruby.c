#include "engine_ruby.h"
#include "sit_engine.h"

VALUE
rbc_engine_new(VALUE class, VALUE rsize) {
	long size = NUM2LONG(rsize);
	sit_engine *engine = sit_engine_new(size);
	VALUE tdata = Data_Wrap_Struct(class, NULL, NULL, engine);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}