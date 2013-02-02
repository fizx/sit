#ifndef QUERY_PARSER_H_INCLUDED
#define QUERY_PARSER_H_INCLUDED

#include "sit_query.h"
#include "sit_callback.h"
#include "pstring.h"
#include <stdbool.h>

typedef struct {
  pstring      *buf;
  pstring      *error;
  int          done;
  sit_callback *cb;
  void         *scanner;
  void         *push_state;
  void         *lvalp;
  void         *llocp;
  char         *ptr;
} query_parser;

typedef enum { NUM, EXPR, CLAUSE, CMP, STR, MODSTR, BOOLOP } query_node_type;
typedef enum { _EQ, _GT, _LT, _GTE, _LTE, _TILDE, _NEQ } cmp_type;

typedef struct query_node {
  query_node_type     type;
  pstring            *val;
  int                 num;
  int                 logic;
  cmp_type            cmp;
  struct query_node  *parent;
  struct query_node  *children;
  struct query_node  *next;
} query_node;

query_parser *
query_parser_new();

int
query_parser_consume(query_parser *parser, pstring *pstr);

void
query_parser_construct(query_parser *context, query_node *root);

#endif