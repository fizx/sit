#include "white_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

typedef struct {
  int token;
  long off; // relative to global stream
  pstring *remaining;
} white_state;

void 
_white_set_off(struct sit_parser *parser, long off) {
  white_state *state = parser->state;
  state->off = off;
}

void
_white_consume(struct sit_parser *parser, pstring *str) {
  white_state *state = parser->state;
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
        parser->receiver->term_found(parser->receiver, term, state->token++);
      } else {
        off = i;
      }
    }
    waswhite = iswhite;
  }
  
  state->off += off;
  
  if(!iswhite) {
    state->remaining = pstring_new2(str->val + off, str->len - off);
  } else {
    state->remaining = pstring_new(0);
  }
}

void white_end_stream(struct sit_parser *parser) {
  assert(parser);
  assert(parser->state);
  white_state *state = parser->state;
  if(state->remaining && state->remaining->len > 0) {
    parser->receiver->term_found(parser->receiver, state->remaining, state->token);
  }
  state->off = 0;
  state->token = 0;
  state->remaining = pstring_new(0);
}

sit_parser *
white_fresh_copy(sit_parser *parser) {
	(void) parser;
	return white_parser_new();
}

sit_parser *
white_parser_new() {
  sit_parser *parser = malloc(sizeof(*parser));
  parser->consume = _white_consume;
  parser->set_offset = _white_set_off;
  parser->state = malloc(sizeof(white_state));
  white_state *state = parser->state;
  state->remaining = NULL;
  parser->end_stream = white_end_stream;
  parser->end_stream(parser);
	parser->fresh_copy = white_fresh_copy;
  return parser;
}