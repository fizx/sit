#ifndef REGEX_PARSER_H_INCLUDED
#define REGEX_PARSER_H_INCLUDED

#include "tokenizer.h"
#include "ast.h"
#include "pstring.h"
#include <pcre.h>
#include <stdbool.h>

#define RE_PARSER_FIELD_LIMIT 64

struct RegexMatcher;

typedef struct RegexParser {
  void               *scanner;
  void               *push_state;
  void               *lvalp;
  void               *llocp;
  bool                done;
  pstring            *buf;
  pstring            *ptr;
  struct RegexMatcher *matcher;
} RegexParser;

typedef struct FieldMatcher {
  pstring *name;
} FieldMatcher;

typedef struct RegexMatcher {
  char        *pattern;
  pcre        *regex;
  pcre_extra  *extra;
  char        *error;
  int          error_offset;  
  int          count;
  FieldMatcher fields[RE_PARSER_FIELD_LIMIT];
} RegexMatcher;

RegexParser *
regex_parser_new();

int
regex_parser_parse(RegexParser *parser, pstring *query);

#endif