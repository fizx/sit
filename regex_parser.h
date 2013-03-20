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
} FieldMatcher;

typedef struct RegexParser {
  void               *scanner;
  void               *push_state;
  void               *lvalp;
  void               *llocp;
  bool                done;
  pstring            *buf;
  pstring            *ptr;
  pstring            *error;
  struct RegexMatcher *matcher;
  
  char        *pattern;
  pcre        *regex;
  pcre_extra  *extra;

  int          count;
  FieldMatcher fields[RE_PARSER_FIELD_LIMIT];
} RegexParser;

RegexParser *
regex_parser_new(pstring *query);

int
regex_parser_consume(RegexParser *parser, pstring *line);

#endif