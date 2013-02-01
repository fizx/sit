#ifndef QUERY_PARSER_H_INCLUDED
#define QUERY_PARSER_H_INCLUDED

#include "sit_query.h"
#include "sit_callback.h"
#include "pstring.h"

typedef struct {
  pstring      *buf;
  pstring      *error;
  int           off;
  sit_callback *cb;
  void         *scanner;
} query_parser;

query_parser *
query_parser_new();

void
query_parser_consume(query_parser *parser, pstring *pstr);

#endif
