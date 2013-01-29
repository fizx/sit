#include "engine_ruby.h"
#include "sit_engine.h"
#include "util_ruby.h"

VALUE
rbc_engine_new(VALUE class, VALUE rparser, VALUE rsize) {
	sit_parser *parser;
	Data_Get_Struct(rparser, sit_parser, parser);
	long size = NUM2LONG(rsize);
	sit_engine *engine = sit_engine_new(parser, size);
	VALUE tdata = Data_Wrap_Struct(class, NULL, NULL, engine);
	rb_obj_call_init(tdata, 0, NULL);
	rb_funcall(rparser, rb_intern("engine="), 1, tdata);
	return tdata;
}

VALUE
rbc_engine_parser(VALUE self) {	
	sit_engine *engine;
	Data_Get_Struct(self, sit_engine, engine);
	sit_parser *parser = engine->parser;
	return vunwrap(parser->data);
}
