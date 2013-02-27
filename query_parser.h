#ifndef QUERY_PARSER_H_INCLUDED
#define QUERY_PARSER_H_INCLUDED

#include "sit.h"

typedef enum { UNKNOWN, NUM, EXPR, CLAUSE, CMP, STR, MODSTR, BAND, BOR, ANDS, ORS, TERM, NUMTERM } query_node_type;
typedef enum { _NA, _EQ, _GT, _LT, _GTE, _LTE, _TILDE, _NEQ } cmp_type;

typedef struct query_node {
  query_node_type     type;
  pstring            *field;
  pstring            *val;
  int                 num;
  cmp_type            cmp;
  bool                negated;
} query_node;

typedef struct query_parser {
  pstring      *buf;
  pstring      *error;
  int          done;
  Callback *cb;
  void         *scanner;
  void         *push_state;
  void         *lvalp;
  void         *llocp;
  void         *tmp;
  char         *ptr;
  Parser   *tokenizer;
  AST        *ast;
  ASTNode   *root;
} query_parser;

query_parser *
query_parser_new();

ASTNode *
query_node_new(query_parser *qp, query_node_type type);

ASTNode *
query_node_copy_subtree(query_parser *parser, ASTNode *subtree);

pstring * 
query_node_to_s(query_node *node);

pstring * 
query_node_query(ASTNode *node);

pstring * 
query_node_ast_to_s(ASTNode *node);

void
query_parser_reset(query_parser *parser);

int
query_parser_consume(query_parser *parser, pstring *pstr);

void
query_parser_construct(query_parser *context, ASTNode *root, int limit);

#endif