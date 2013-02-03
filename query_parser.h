#ifndef QUERY_PARSER_H_INCLUDED
#define QUERY_PARSER_H_INCLUDED

#include "sit_query.h"
#include "sit_callback.h"
#include "pstring.h"
#include <stdbool.h>
#include "ast.h"

typedef enum { UNKNOWN, NUM, EXPR, CLAUSE, CMP, STR, MODSTR, BAND, BOR, ANDS, ORS } query_node_type;
typedef enum { _NA, _EQ, _GT, _LT, _GTE, _LTE, _TILDE, _NEQ } cmp_type;

typedef struct query_node {
  query_node_type     type;
  pstring            *val;
  int                 num;
  cmp_type            cmp;
  bool                negated;
} query_node;

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
  ast_t        *ast;
  ast_node_t   *root;
} query_parser;

query_parser *
query_parser_new();

ast_node_t *
query_node_new(query_parser *qp, query_node_type type);

ast_node_t *
query_node_copy_subtree(query_parser *parser, ast_node_t *subtree);

pstring * 
query_node_to_s(query_node *node);

pstring * 
query_node_query(ast_node_t *node);

int
query_parser_consume(query_parser *parser, pstring *pstr);

void
query_parser_construct(query_parser *context, ast_node_t *root);

#endif