#include "sit_ruby.h"

void 
_rbc_consume(Parser *parser, pstring *pstr) {  
	VALUE rparser = vunwrap(parser->data);
	VALUE rstr = p2rstring(pstr);
	rb_funcall(rparser, rb_intern("consume"), 1, rstr);
}

Parser *
rbc_regex_fresh_copy(Parser *parser) {
  Parser* copy = regex_parser_fresh_copy(parser);
	copy->fresh_copy = rbc_regex_fresh_copy;
	VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::Parser"), markall, NULL, copy);
	rb_obj_call_init(tdata, 0, NULL);
  SET_ONCE(copy->data, vwrap(tdata));
  return copy;
}

Parser *
rbc_default_fresh_copy(Parser *parser) {
  VALUE old = vunwrap(parser->data);
  VALUE class = rb_funcall(old, rb_intern("class"), 0);
  Parser* copy = default_fresh_copy(parser);
	copy->consume = _rbc_consume;
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
rbc_syslog_fresh_copy(Parser *parser) {
  Parser* copy = syslog_fresh_copy(parser);
	copy->fresh_copy = rbc_syslog_fresh_copy;
	VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::Parser"), markall, NULL, copy);
	rb_obj_call_init(tdata, 0, NULL);
  SET_ONCE(copy->data, vwrap(tdata));
  return copy;
}

Parser *
rbc_solr_fresh_copy(Parser *parser) {
  Parser* copy = solr_fresh_copy(parser);
	copy->fresh_copy = rbc_solr_fresh_copy;
	VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::Parser"), markall, NULL, copy);
	rb_obj_call_init(tdata, 0, NULL);
  SET_ONCE(copy->data, vwrap(tdata));
  return copy;
}

Tokenizer *
rbc_white_fresh_copy(Tokenizer *tok) {
  Tokenizer* copy = white_fresh_copy(tok);
	copy->fresh_copy = rbc_white_fresh_copy;
	VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::Parser"), markall, NULL, copy);
	rb_obj_call_init(tdata, 0, NULL);
  SET_ONCE(copy->data, vwrap(tdata));
  return copy;
}

VALUE 
rbc_parser_new(VALUE class) {
	Parser *parser = parser_new();
	parser->consume = _rbc_consume;
	parser->fresh_copy = rbc_default_fresh_copy;
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, parser);
	rb_obj_call_init(tdata, 0, NULL);
  SET_ONCE(parser->data, vwrap(tdata));
	return tdata;
}

VALUE
rbc_parser_new_regex(VALUE class, VALUE rspec) {
  pstring *spec = r2pstring(spec);
	Parser *parser = regex_parser_new(spec);
  pstring_free(spec);
  parser->fresh_copy = rbc_regex_fresh_copy;
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, parser);
	rb_obj_call_init(tdata, 0, NULL);
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
rbc_parser_new_solr(VALUE class) {
	Parser *parser = solr_parser_new();
  parser->fresh_copy = rbc_solr_fresh_copy;
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, parser);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE 
rbc_parser_new_syslog(VALUE class) {
	Parser *parser = syslog_simple_parser_new();
  parser->fresh_copy = rbc_syslog_fresh_copy;
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, parser);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

void 
_rb_ondoc(Callback *self, void *data) {
  VALUE class = rb_eval_string("::Sit::DocBuf");
  VALUE block = vunwrap(self->user_data);
  VALUE tdata = Data_Wrap_Struct(class, markall, NULL, data);
	rb_obj_call_init(tdata, 0, NULL);  	
  rb_funcall(block, rb_intern("call"), 1, tdata);
}

VALUE
rbc_parser_on_document(VALUE self, VALUE proc) {
  Parser *parser;
	Data_Get_Struct(self, Parser, parser);
  parser->on_document = callback_new(_rb_ondoc, vwrap(proc));
  return Qnil;
}

VALUE
rbc_parser_consume(VALUE self, VALUE rstr) {
	Parser *parser;
	Data_Get_Struct(self, Parser, parser);
	if(!parser->on_document) {
		rb_raise(rb_eRuntimeError, "no on_document");
    return Qnil;
	}
  pstring *pstr = r2pstring(rstr);
  parser->consume(parser, pstr);
  pstring_free(pstr);
	return Qnil;
}

VALUE
rbc_parser_end_stream(VALUE self) {
  Parser *parser;
	Data_Get_Struct(self, Parser, parser);
  parser->end_stream(parser);
	return Qnil;
}