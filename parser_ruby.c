#include "sit_ruby.h"

void 
rbc_consume(Parser *parser, pstring *pstr) {  
	VALUE rparser = vunwrap(parser->data);
	VALUE rstr = p2rstring(pstr);
	rb_funcall(rparser, rb_intern("consume"), 1, rstr);
}

Parser *
rbc_default_fresh_copy(Parser *parser) {
  VALUE old = vunwrap(parser->data);
  VALUE class = rb_funcall(old, rb_intern("class"), 0);
  Parser* copy = default_fresh_copy(parser);
	copy->consume = rbc_consume;
	copy->fresh_copy = default_fresh_copy;
	
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, copy);
	rb_obj_call_init(tdata, 0, NULL);
  copy->data = vwrap(tdata);
  return copy;
}

Parser *
rbc_json_fresh_copy(Parser *parser) {
  Parser* copy = json_fresh_copy(parser);
	copy->fresh_copy = rbc_json_fresh_copy;
	VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::Parser"), markall, NULL, copy);
	rb_obj_call_init(tdata, 0, NULL);
  SET_ONCE(copy->data, vwrap(tdata));
  return copy;
}

Parser *
rbc_white_fresh_copy(Parser *parser) {
  Parser* copy = white_fresh_copy(parser);
	copy->fresh_copy = rbc_white_fresh_copy;
	VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::Parser"), markall, NULL, copy);
	rb_obj_call_init(tdata, 0, NULL);
  SET_ONCE(copy->data, vwrap(tdata));
  return copy;
}

VALUE 
rbc_parser_new(VALUE class) {
	Parser *parser = sit_parser_new();
	parser->consume = rbc_consume;
	parser->fresh_copy = rbc_default_fresh_copy;
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, parser);
	rb_obj_call_init(tdata, 0, NULL);
  SET_ONCE(parser->data, vwrap(tdata));
	return tdata;
}

VALUE 
rbc_parser_new_json(VALUE class) {
	Parser *parser = json_white_parser_new();
  parser->fresh_copy = rbc_json_fresh_copy;
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, parser);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE 
rbc_parser_new_whitespace(VALUE class) {
	Parser *parser = white_parser_new();
  parser->fresh_copy = rbc_white_fresh_copy;
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, parser);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE
rbc_parser_set_receiver(VALUE self, VALUE robj) {
	if(!rb_obj_is_kind_of(robj, rb_eval_string("::Sit::Forwardable"))) {
		rb_raise(rb_eRuntimeError, "not forwardable");
		return Qnil;
	}
	Parser *parser;
	Data_Get_Struct(self, Parser, parser);
	void *receiver;
	Data_Get_Struct(robj, void, receiver);
	parser->receiver = receiver;
	return Qnil;	
}

VALUE
rbc_parser_consume(VALUE self, VALUE rstr) {
	Parser *parser;
	Data_Get_Struct(self, Parser, parser);
	if(!parser->receiver) {
		rb_raise(rb_eRuntimeError, "no receiver");
    return Qnil;
	}
  pstring *pstr = r2pstring(rstr);
  parser->consume(parser, pstr);
  pstring_free(pstr);
	return Qnil;
}

VALUE
rbc_parser_term_found(VALUE self, VALUE rstr, VALUE rfield_offset) { 
	Parser *parser;
	Data_Get_Struct(self, Parser, parser);
  pstring *pstr = r2pstring(rstr);
  int field_offset = NUM2INT(rfield_offset);
  parser->receiver->term_found(parser->receiver, pstr, field_offset);
	return Qnil;
}

VALUE 
rbc_parser_int_found(VALUE self, VALUE rint) {
	Parser *parser;
	Data_Get_Struct(self, Parser, parser);
  int val = NUM2INT(rint);
  parser->receiver->int_found(parser->receiver, val);
	return Qnil;
}

VALUE
rbc_parser_document_found(VALUE self, VALUE roff, VALUE rlen) {
	Parser *parser;
	Data_Get_Struct(self, Parser, parser);
  long off = NUM2LONG(roff);
  int len = NUM2INT(rlen);
  parser->receiver->document_found(parser->receiver, off, len);
	return Qnil;
}

VALUE
rbc_parser_end_stream(VALUE self) {
  Parser *parser;
	Data_Get_Struct(self, Parser, parser);
  parser->end_stream(parser);
	return Qnil;
}

VALUE 
rbc_parser_field_found(VALUE self, VALUE rstr) {
  Parser *parser;
	Data_Get_Struct(self, Parser, parser);
  pstring *pstr = r2pstring(rstr);
  parser->receiver->field_found(parser->receiver, pstr);
	return Qnil;
}
