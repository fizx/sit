#include "white_parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

void
_white_consume(struct sit_parser *parser, pstring *str) {
  printf("ENTER\n");
  int token = 0;
  int off = 0;
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
        parser->term_found(parser, off, i - off, token++);
      } else {
        off = i;
      }
    }
    waswhite = iswhite;
  }
  if(!iswhite) {
    parser->term_found(parser, off, str->len - off, token++);
  }
}

sit_parser *
white_parser_new() {
  sit_parser *parser = malloc(sizeof(*parser));
  parser->consume = _white_consume;
  return parser;
}