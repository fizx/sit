#ifndef QUERY_PARSER_H_INCLUDED
#define QUERY_PARSER_H_INCLUDED

#include "tokenizer.h"
#include "ast.h"
#include "pstring.h"

#include <stdbool.h>

typedef enum { UNKNOWN, NUM, EXPR, CLAUSE, CMP, STR, MODSTR, BAND, BOR, ANDS, ORS, TERM, NUMTERM } QNodeType;
typedef enum { _NA, _EQ, _GT, _LT, _GTE, _LTE, _TILDE, _NEQ } CmpType;

typedef struct QNode {
  QNodeType     type;
  pstring          *field;
  pstring          *val;
  int               num;
  CmpType           cmp;
  bool              negated;
} QNode;

typedef struct QueryParser {
  pstring      *buf;
  pstring      *error;
  int           done;
  Callback     *on_query;
  void         *scanner;
  void         *push_state;
  void         *lvalp;
  void         *llocp;
  void         *tmp;
  char         *ptr;
  Tokenizer    *tokenizer;
  AST          *ast;
  ASTNode      *root;
} QueryParser;

QueryParser *
query_parser_new();

void 
qp_freeable(QueryParser *qp, void *data);

void
query_parser_free(QueryParser *qp);

ASTNode *
ast_query_node_new(QueryParser *qp, QNodeType type);

ASTNode *
query_node_copy_subtree(QueryParser *parser, ASTNode *subtree);

pstring * 
query_node_to_s(QNode *node);

pstring * 
query_node_query(ASTNode *node);

pstring * 
query_node_ast_to_s(ASTNode *node);

void
query_parser_reset(QueryParser *parser);

int
query_parser_consume(QueryParser *parser, pstring *pstr);

void
query_parser_construct(QueryParser *context, ASTNode *root, int limit);

#endif