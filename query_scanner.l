%option reentrant
%option bison-bridge
%option bison-locations
%option noyywrap
%option yylineno

D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
NS    [^ \t\n\r,?*();:\^"]

%{
#include "y.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "query_parser.h"
#include "pstring.h"
#include <assert.h>

#define YY_EXTRA_TYPE query_parser*
#define YY_USER_ACTION yylloc->first_line = yylineno;

#define YY_INPUT(cbuf, offset, max_size)  {                                   \
  int i;\
  for(i = 0; i < max_size && offset < yyextra->buf->len; i++, offset++) { \
    cbuf[i] = yyextra->buf->val[offset];                                      \
  }                                                                         \
  if(i == 0) {                                                              \
    yyextra->done++;                                                   \
  }                                                                         \
  offset = i;                                                                 \
}                                                                             

%}

%%

("and"|"AND")             { return(AND); }
("or"|"OR")               { return(OR); }
("not"|"NOT")             { return(NOT); }
"("                       { return(LPAREN); }
")"                       { return(RPAREN); }
"="                       { return(EQ); }
">"                       { return(GT); }
"<"                       { return(LT); }
">="                      { return(GTE); }
"<="                      { return(LTE); }
"~"                       { return(TILDE); }
"<>"                      { return(NEQ); }
"-"                       { return(MINUS); }
{D}+                      { yyextra->ptr = cstring_new(yytext, yyleng); return(DIGITS); }
"."                       { return(DOT); }
L?\"(\\.|[^\\"])*\"	      { yyextra->ptr = cstring_new(yytext, yyleng); return(STRING_LITERAL); }
{NS}+             	      { yyextra->ptr = cstring_new(yytext, yyleng); return(UNQUOTED); }
[;\n]+                    { return(EOQ); }
[ \t\v\f]  		            { /* ignore whitespace*/ }
.			                    { /* ignore bad characters */ }

%%

ast_node_t *
query_node_new(query_parser *qp, query_node_type type) {
  query_node *node = malloc(sizeof(query_node));
  node->type     = type;
  node->val      = NULL;
  node->num      = 0;
  node->negated  = false;
  
  ast_node_t *ast_node = ast_node_new(qp->ast);
  ast_node->data = node;
  
  return ast_node;
}

query_parser *
query_parser_new(sit_callback *cb) {
  query_parser *parser = malloc(sizeof(query_parser));
  parser->buf = NULL;
  parser->error = NULL;
  parser->ast = ast_new(NULL, free);
  parser->root = NULL;
  parser->cb = cb;
  parser->push_state = yypstate_new();
  parser->lvalp = malloc(sizeof(YYSTYPE));
  parser->llocp = malloc(sizeof(YYLTYPE));
  yylex_init(&parser->scanner);
  yyset_extra(parser, parser->scanner);
  return parser;
}

int
query_parser_consume(query_parser *parser, pstring *pstr) {
  assert(pstr);
  assert(pstr->len);
  parser->buf = pstr;
  parser->done = false;
  //return yyparse(parser);
  int status = YYPUSH_MORE;
  while (status == YYPUSH_MORE && parser->done < 1) {
    status = yypush_parse(
      parser->push_state,
      yylex(parser->lvalp, parser->llocp, parser->scanner), 
      parser->lvalp,
      parser->llocp,
      parser);
  }
  return status;
}

void 
associate_ands(ast_node_t *node) {
  // query_node *qn = node->data;
  // if(!expr){// || (expr->type != EXPR && expr->type != CLAUSE)) {
  // printf("FUCK %d\n", expr && expr->type == CLAUSE);
  //   return;
  // }  
  // 
  // printf("down\n");
  // associate_ands(expr->children);
  // printf("up\n");
  // 
  // query_node *op = expr->next;
  // query_node *other = op == NULL ? NULL : op->next;
  // query_node *after = other == NULL ? NULL : other->next;
  // 
  // if(op != NULL && op->type == BAND) {  
  //   printf("splice\n");
  //   query_node *p = query_node_new(ANDS);
  //   p->parent = expr->parent;
  //   if(p->parent && p->parent->children == expr) p->parent->children = p;
  //   p->prev = expr->prev;
  //   if(p->prev) p->prev->next = p;
  //   p->next = after;
  //   if(after) after->prev = p;
  //   p->children = expr;
  //   expr->parent = p;
  //   expr->prev = NULL;
  //   expr->next = other;
  //   other->next = NULL;
  //   if(expr->next) expr->next->prev = expr;
  // 
  //   pstring *pstr = query_node_to_s(p);
  //   printf("HERE: %.*s\n", pstr->len, pstr->val);
  //   
  //   pstr = query_node_to_s(p->prev);
  //   printf("PREV: %.*s\n", pstr->len, pstr->val);
  //   
  //   printf("AFTER: %d\n", (long) p->next);
  // 
  //   associate_ands(p);
  //   
  //   printf("WUT\n");
  // } else {
  //   printf("ZOMG %d\n", (long)other);
  //   
  //   associate_ands(other);
  // }
}

void
query_parser_construct(query_parser *context, ast_node_t *expression) {
  context->root = expression;
  // associate_ands(expression);
}

char *
_s(query_node_type t) {
  switch(t) {
    case UNKNOWN : return "UNKNOWN" ;
    case NUM     : return "NUM"     ;
    case EXPR    : return "EXPR"    ;
    case CLAUSE  : return "CLAUSE"  ;
    case CMP     : return "CMP"     ;
    case STR     : return "STR"     ;
    case MODSTR  : return "MODSTR"  ;
    case BAND    : return "BAND"    ;
    case BOR     : return "BOR"     ;
    case ANDS    : return "ANDS"    ;
    case ORS     : return "ORS"     ;
  }
}

char *
_c(cmp_type t) {
  switch(t) {
    case _NA    : return "_NA";
    case _EQ    : return "_EQ";
    case _GT    : return "_GT";
    case _LT    : return "_LT";
    case _GTE   : return "_GTE";
    case _LTE   : return "_LTE";
    case _TILDE : return "_TILDE";
    case _NEQ   : return "_NEQ";
  }
}

pstring * 
query_node_to_s(query_node *node) {
  char *str;
  int len = 4;
  char *val = "NULL";
  if(node->val) {
    len = node->val->len;
    val = node->val->val;
  }
  asprintf(&str, "[%s %.*s %d %s %s]", _s(node->type), len, val, node->num, _c(node->cmp), node->negated ? "true" : "false");
  pstring *pstr = malloc(sizeof(*pstr));
  pstr->val = str;
  pstr->len = strlen(str);
  return pstr;
}