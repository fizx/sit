#ifndef REGEX_PARSER_H_INCLUDED
#define REGEX_PARSER_H_INCLUDED

#include "tokenizer.h"
#include "ast.h"
#include "pstring.h"
#include "vstring.h"
#include "parser.h"
#include "../contrib/pcre/pcre.h"
#include <stdbool.h>

#define RE_PARSER_FIELD_LIMIT 64

typedef enum { STRING, INT, TOKENS } FieldType;

typedef struct FieldMatcher {
  pstring   *name;
  pstring   *alias;
  int        group;
  FieldType  type;
  Tokenizer *tokenizer;
} FieldMatcher;

typedef struct RegexParserState {
  Parser              *parser;
  void                *scanner;
  void                *lvalp;
  void                *llocp;
  pstring             *buf;
  pstring             *ptr;
  pstring             *error;
  struct RegexMatcher *matcher;
  
  vstring             *stream;
  
  char *re_error;
  int   re_error_offset;
  
  pstring             *pattern;
  pcre                *regex;
  pcre_extra          *extra;
  
  LList *freeable;

  int                  count;
  FieldMatcher         fields[RE_PARSER_FIELD_LIMIT];
} RegexParserState;

Parser *
regex_parser_new(pstring *query);

Parser *
regex_parser_fresh_copy(Parser *parser);

void
regex_parser_consume(Parser *parser, pstring *line);

#endif