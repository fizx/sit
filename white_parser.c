#include "white_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct {
  int token;
  long off;
  pstring *remaining;
} white_state;

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
        parser->term_found(parser, state->off + off, i - off, state->token++);
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
  white_state *state = parser->state;
  if(state->remaining && state->remaining->len > 0) {
    parser->term_found(parser, state->off, state->remaining->len, state->token);
  }
  state->off = 0;
  state->token = 0;
  state->remaining = pstring_new(0);
}

sit_parser *
white_parser_new() {
  sit_parser *parser = malloc(sizeof(*parser));
  parser->consume = _white_consume;
  parser->state = malloc(sizeof(white_state));
  white_state *state = parser->state;
  state->remaining = NULL;
  parser->end_stream = white_end_stream;
  parser->end_stream(parser);
  return parser;
}