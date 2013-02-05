#include "tokenizer_ruby.h"
#include "util_ruby.h"
#include "pstring_ruby.h"
#include "sit_parser.h"
#include "white_parser.h"

void 
_tok_term_found(struct sit_parser *parser, long off, int len, int field_offset) {  
  VALUE rparser = vunwrap(parser->data);
  VALUE roff = LONG2NUM(off);
  VALUE rlen = INT2NUM(len);
  VALUE rfield_offset = INT2NUM(field_offset);
  rb_funcall(rparser, rb_intern("term_found"), 3, roff, rlen, rfield_offset);
}

VALUE 
rbc_tokenizer_new(VALUE class) {
	sit_parser *parser = white_parser_new();
	VALUE tdata = Data_Wrap_Struct(class, NULL, NULL, parser);
	rb_obj_call_init(tdata, 0, NULL);
  parser->data = vwrap(tdata);
  parser->term_found = _tok_term_found;  
  return tdata;
}

VALUE 
rbc_tokenizer_consume(VALUE self, VALUE rstr) {
  sit_parser *parser;
	Data_Get_Struct(self, sit_parser, parser);
  pstring *pstr = r2pstring(rstr);
  parser->consume(parser, pstr);
  pstring_free(pstr);
	return Qnil;
}

VALUE 
rbc_tokenizer_term_found(VALUE self, VALUE roff, VALUE rlen, VALUE rfield_offset) {
  (void) self;
  (void) roff;
  (void) rlen;
  (void) rfield_offset;
	return Qnil;
}

VALUE
rbc_tokenizer_end_stream(VALUE self) {
  sit_parser *parser;
	Data_Get_Struct(self, sit_parser, parser);
  parser->end_stream(parser);
	return Qnil;
}
