#ifndef REGEX_PARSER_H_INCLUDED
#define REGEX_PARSER_H_INCLUDED

#include "tokenizer.h"
#include "ast.h"
#include "pstring.h"
#include <pcre.h>
#include <stdbool.h>

#define RE_PARSER_FIELD_LIMIT 64

typedef struct FieldMatcher {
  pstring *name;
  pstring *type;
} FieldMatcher;

typedef struct RegexParser {
  void                *scanner;
  void                *push_state;
  void                *lvalp;
  void                *llocp;
  bool                 done;
  pstring             *buf;
  pstring             *ptr;
  pstring             *error;
  struct RegexMatcher *matcher;
  
  pstring             *pattern;
  pcre                *regex;
  pcre_extra          *extra;
  
  LList *freeable;

  int                  count;
  FieldMatcher         fields[RE_PARSER_FIELD_LIMIT];
} RegexParser;

RegexParser *
regex_parser_new(pstring *query);

RegexParser *
regex_parser_fresh_copy(RegexParser *parser);

int
regex_parser_consume(RegexParser *parser, pstring *line);

#endif