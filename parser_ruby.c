#include "parser_ruby.h"
#include "pstring.h"
#include "pstring_ruby.h"
#include "assert.h"
#include "sit_engine.h"
#include "sit_parser.h"
#include "util_ruby.h"

void 
rbc_consume(sit_parser *parser, pstring *pstr) {
	VALUE rparser = vunwrap(parser->data);
	VALUE rstr = p2rstring(pstr);
	rb_funcall(rparser, rb_intern("consume"), 1, rstr);
}
	
void 
_ruby_term_found(struct sit_parser *parser, long off, int len, int field_offset) {
  VALUE rparser = vunwrap(parser->data);
  VALUE rengine = rb_iv_get(rparser, "@engine");
	sit_engine *engine;
	Data_Get_Struct(rengine, sit_engine, engine);
	sit_engine_term_found(engine, off, len, field_offset);
}

void 
_ruby_document_found(struct sit_parser *parser, long off, int len) {
  VALUE rparser = vunwrap(parser->data);
  VALUE rengine = rb_iv_get(rparser, "@engine");
	sit_engine *engine;
	Data_Get_Struct(rengine, sit_engine, engine);
  sit_engine_document_found(engine, off, len);
}

void 
_ruby_field_found(struct sit_parser *parser, pstring *name) {
  VALUE rparser = vunwrap(parser->data);
  VALUE rengine = rb_iv_get(rparser, "@engine");
	sit_engine *engine;
	Data_Get_Struct(rengine, sit_engine, engine);
  sit_engine_field_found(engine, name);
}

VALUE 
rbc_parser_new(VALUE class) {
	sit_parser *parser = sit_parser_new(NULL);
	VALUE tdata = Data_Wrap_Struct(class, NULL, NULL, parser);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE 
rbc_parser_initialize(VALUE self) {	
	sit_parser *parser;
	Data_Get_Struct(self, sit_parser, parser);
	parser->consume = rbc_consume;
	parser->data = (void *) vwrap(self);
  parser->term_found = _ruby_term_found;
  parser->document_found = _ruby_document_found;
  parser->field_found = _ruby_field_found;
	return self;
}

VALUE
rbc_parser_set_engine(VALUE self, VALUE rengine) {
	return rb_iv_set(self, "@engine", rengine);
}

VALUE 
rbc_parser_engine(VALUE self) {
	return rb_iv_get(self, "@engine");
}


VALUE
rbc_parser_consume(VALUE self, VALUE rstr) {
	rb_raise(rb_eRuntimeError, "You should override Parser#consume in a subclass");
	(void) self; 
	(void) rstr;
	return Qnil;
}

VALUE
rbc_parser_term_found(VALUE self, VALUE roff, VALUE rlen, VALUE rfield_offset){ 
	sit_parser *parser;
	Data_Get_Struct(self, sit_parser, parser);
	long off = NUM2LONG(roff);
	int len = NUM2INT(rlen);
	int field_offset = NUM2INT(rfield_offset);
  parser->term_found(parser, off, len, field_offset);
	return Qnil;
}

VALUE 
rbc_parser_int_found(VALUE self, VALUE rint) {
	VALUE rengine = rb_iv_get(self, "@engine");
	sit_engine *engine;
	Data_Get_Struct(rengine, sit_engine, engine);
	int value = NUM2INT(rint);
	sit_engine_int_found(engine, value);
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
