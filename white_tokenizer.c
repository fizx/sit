#include "sit.h"

typedef struct {
  pstring *remaining;
  int offset;
} WhiteState;

void
_white_consume(Tokenizer *tok, pstring *str) {
  DEBUG("tokenizing %.*s", str->len, str->val);
  WhiteState *state = tok->state;
  padd(state->remaining, str);
  str = state->remaining;
  long off = 0;
  bool waswhite = false;
  bool iswhite = false;
  char c;
  for (int i =0; i < str->len; i++) {
    c = str->val[i];
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
        pstring *term = pstring_new2(str->val + off, i-off);
        Token token = {
          term, 
          state->offset++
        };
        DEBUG("have token: %.*s %d", term->len, term->val, state->offset);
        tok->on_token->handler(tok->on_token, &token);
      } else {
        off = i;
      }
    }
    waswhite = iswhite;
  }
    
  if(!iswhite) {
    state->remaining = pstring_new2(str->val + off, str->len - off);
  } else {
    state->remaining = pstring_new(0);
  }
}

void white_end_stream(Tokenizer *tok) {
  assert(tok);
  assert(tok->state);
  WhiteState *state = tok->state;
  if(state->remaining && state->remaining->len > 0) {
    Token token = {
      state->remaining,
      state->offset++
    };
    DEBUG("have token: %.*s %d", state->remaining->len, state->remaining->val, state->offset);
    tok->on_token->handler(tok->on_token, &token);
  }
  state->remaining = pstring_new(0);
  state->offset = 0;
}

Tokenizer *
white_fresh_copy(Tokenizer *t) {
	(void) t;
	return white_tokenizer_new();
}

Tokenizer *
white_tokenizer_new() {
  Tokenizer *tok = calloc(1, sizeof(*tok));
  tok->consume = _white_consume;
  tok->state = calloc(1, sizeof(WhiteState));
  WhiteState *state = tok->state;
  state->remaining = pstring_new(0);
  tok->end_stream = white_end_stream;
	tok->fresh_copy = white_fresh_copy;
  return tok;
}