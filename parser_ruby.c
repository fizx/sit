#include "parser_ruby.h"
#include "pstring.h"
#include "pstring_ruby.h"
#include "assert.h"
#include "sit_engine.h"
#include "sit_parser.h"
#include "util_ruby.h"

void 
rbc_consume(void *data, pstring *pstr) {
	sit_parser *parser = data;
	VALUE rparser = vunwrap(parser->data);
	VALUE rstr = p2rstring(pstr);
	rb_funcall(rparser, rb_intern("consume"), 1, rstr);
}

VALUE 
rbc_parser_new(VALUE class) {
	sit_parser *parser = sit_parser_new();
	VALUE tdata = Data_Wrap_Struct(class, NULL, NULL, parser);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE 
rbc_parser_initialize(VALUE self) {	
	sit_parser *parser;
	Data_Get_Struct(self, sit_parser, parser);
	parser->consume = rbc_consume;
	parser->data = vwrap(self);
	return self;
}

VALUE
rbc_parser_set_engine(VALUE self, VALUE rengine) {
	return rb_iv_set(self, "@engine", rengine);
}

VALUE
rbc_parser_consume(VALUE self, VALUE rstr) {
	rb_raise(rb_eRuntimeError, "You should override Parser#consume in a subclass");

	// kill warns
	StringValue(self); 
	StringValue(rstr);

	return Qnil;
}

VALUE
rbc_parser_term_found(VALUE self, VALUE roff, VALUE rlen, VALUE rfield_offset){ 
	VALUE rengine = rb_iv_get(self, "@engine");
	sit_engine *engine;
	Data_Get_Struct(rengine, sit_engine, engine);
	long off = NUM2LONG(roff);
	int len = NUM2INT(rlen);
	int field_offset = NUM2INT(rfield_offset);
	sit_engine_term_found(engine, off, len, field_offset);
	return Qnil;
}

VALUE
rbc_parser_document_found(VALUE self, VALUE roff, VALUE rlen) {
	VALUE rengine = rb_iv_get(self, "@engine");
	sit_engine *engine;
	Data_Get_Struct(rengine, sit_engine, engine);
	long off = NUM2LONG(roff);
	int len = NUM2INT(rlen);
	sit_engine_document_found(engine, off, len);
	return Qnil;
}

VALUE 
rbc_parser_field_found(VALUE self, VALUE rstr) {
	VALUE rengine = rb_iv_get(self, "@engine");
	sit_engine *engine;
	Data_Get_Struct(rengine, sit_engine, engine);
	pstring *pstr = r2pstring(rstr);
	sit_engine_field_found(engine, pstr);
	return Qnil;
}
