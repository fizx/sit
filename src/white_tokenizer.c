#include "sit.h"
#include "tokenizer.h"

void
_white_consume(Tokenizer *tok, pstring *str) {
  int len = str->len;
  const char *val = str->val;
  DEBUG("tokenizing %.*s", str->len, str->val);
  long off = 0;
  bool waswhite = false;
  bool iswhite = false;
  char c;
  int offset = 0;
  for (int i = 0; i < len; i++) {
    c = val[i];
    switch(c) {
    case '\t':
    case ' ':
    case '\v':
    case '\r':
    case '\n':
      iswhite = true;
      break;
    default: 
      iswhite = false;
    }
    
    if(iswhite != waswhite) {
      if(iswhite) {
        pstring term = {
          val + off, 
          i-off
        };
        Token token = {
          &term, 
          offset++
        };
        DEBUG("have token: %.*s %d", term.len, term.val, offset);
        tok->on_token->handler(tok->on_token, &token);
      } else {
        off = i;
      }
    }
    waswhite = iswhite;
  }
    
  if(!iswhite) {
    pstring term = {
     val + off, 
     len - off
    };
    Token token = {
      &term, 
      offset
    };
    tok->on_token->handler(tok->on_token, &token);
  }
}

Tokenizer *
white_fresh_copy(Tokenizer *t) {
	(void) t;
	return white_tokenizer_new();
}

void
_white_tokenizer_free(void *data) {
  Tokenizer *tok = data;
  free(tok);
}

Tokenizer *
white_tokenizer_new() {
  Tokenizer *tok = tokenizer_new();
  tok->consume = _white_consume;
	tok->fresh_copy = white_fresh_copy;
  return tok;
}