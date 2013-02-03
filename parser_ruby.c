#include "parser_ruby.h"
#include "pstring.h"
#include "pstring_ruby.h"
#include "assert.h"
#include "sit_engine.h"
#include "sit_parser.h"
#include "util_ruby.h"
#include "white_parser.h"

void 
rbc_consume(sit_parser *parser, pstring *pstr) {  
	VALUE rparser = vunwrap(parser->data);
	VALUE rstr = p2rstring(pstr);
	rb_funcall(rparser, rb_intern("consume"), 1, rstr);
}
	
void 
_ruby_term_found(struct sit_parser *parser, long off, int len, int field_offset) {  
  VALUE rparser = vunwrap(parser->data);
  VALUE roff = LONG2NUM(off);
  VALUE rlen = INT2NUM(len);
  VALUE rfield_offset = INT2NUM(field_offset);
  rb_funcall(rparser, rb_intern("term_found"), 3, roff, rlen, rfield_offset);
}

void 
_ruby_document_found(struct sit_parser *parser, long off, int len) {
  VALUE rparser = vunwrap(parser->data);
  VALUE roff = LONG2NUM(off);
  VALUE rlen = INT2NUM(len);
  rb_funcall(rparser, rb_intern("document_found"), 2, roff, rlen);
}

void 
_ruby_field_found(struct sit_parser *parser, pstring *name) {
  VALUE rparser = vunwrap(parser->data);
  VALUE rstr = p2rstring(name);
  rb_funcall(rparser, rb_intern("field_found"), 1, rstr);
}

VALUE 
rbc_parser_new(VALUE class) {
	sit_parser *parser = sit_parser_new(NULL);
	parser->consume = rbc_consume;
	VALUE tdata = Data_Wrap_Struct(class, NULL, NULL, parser);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE 
rbc_whitespace_parser_new(VALUE class) {
	sit_parser *parser = white_parser_new();
	VALUE tdata = Data_Wrap_Struct(class, NULL, NULL, parser);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE 
rbc_parser_initialize(VALUE self) {	
	sit_parser *parser;
	Data_Get_Struct(self, sit_parser, parser);
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
	sit_parser *parser;
	Data_Get_Struct(self, sit_parser, parser);
  pstring *pstr = r2pstring(rstr);
  parser->consume(parser, pstr);
  pstring_free(pstr);
	return Qnil;
}

VALUE
rbc_parser_term_found(VALUE self, VALUE roff, VALUE rlen, VALUE rfield_offset){ 
  VALUE rengine = rb_iv_get(self, "@engine");
  rb_funcall(rengine, rb_intern("term_found"), 3, roff, rlen, rfield_offset);
	return Qnil;
}

VALUE 
rbc_parser_int_found(VALUE self, VALUE rint) {
  VALUE rengine = rb_iv_get(self, "@engine");
  rb_funcall(rengine, rb_intern("int_found"), 1, rint);
	return Qnil;
}

VALUE
rbc_parser_document_found(VALUE self, VALUE roff, VALUE rlen) {
  VALUE rengine = rb_iv_get(self, "@engine");
  rb_funcall(rengine, rb_intern("document_found"), 2, roff, rlen);
	return Qnil;
}

VALUE 
rbc_parser_field_found(VALUE self, VALUE rstr) {
  VALUE rengine = rb_iv_get(self, "@engine");
  rb_funcall(rengine, rb_intern("field_found"), 1, rstr);
	return Qnil;
}
