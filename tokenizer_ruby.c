#include "sit_ruby.h"

void
_rbc_on_token(Callback *cb, void *data) {
  VALUE tokens = vunwrap(cb->user_data);
  Token *token = data;
  VALUE rstr = p2rstring(token->text);
  rb_ary_push(tokens, rstr);
}

VALUE 
rbc_tokenizer_new_whitespace(VALUE class, VALUE tokens) {
  Tokenizer *tok = white_tokenizer_new();
  tok->on_token = callback_new(_rbc_on_token, vwrap(tokens));
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, tok);
	rb_obj_call_init(tdata, 0, NULL);
  return tdata;
}

VALUE
rbc_tokenizer_consume(VALUE self, VALUE rstr) {
  Tokenizer *tok;
	Data_Get_Struct(self, Tokenizer, tok);
  tok->consume(tok, r2pstring(rstr));
  return Qnil;
}