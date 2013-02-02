
%token_table
%debug
%locations
%defines
%error-verbose

%define api.pure full
%define api.push-pull both

%parse-param { query_parser* context }
%lex-param { void* scanner  }

%code requires {
  #include "query_parser.h"
}

%union
{ 
  query_node *node;
  int         num;
  char       *cptr;
  cmp_type    cmp;
}


%{

#include "query_parser.h"
#include "sit_callback.h"
#include "pstring.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int yylex(YYSTYPE* lvalp, YYLTYPE* llocp, void* scanner);

void yyerror(YYLTYPE* locp, query_parser *parser, const char* err) {
  (void) locp;
  parser->error = c2pstring(err);
}

#define scanner context->scanner

query_node *
query_node_new(query_node_type type) {
  query_node *node = malloc(sizeof(query_node));
  node->type     = type;
  node->val      = NULL;
  node->num      = 0;
  node->logic    = 0;
  node->parent   = NULL;
  node->children = NULL;
  node->next     = NULL;
  return node;
}

query_node *
int_node(char *str) {
  query_node *node = query_node_new(NUM);
  node->num = atoi(str);
  return node;
}

query_node *
str_node(char *str) {
  query_node *node = query_node_new(STR);
  node->val = c2pstring(str);
  return node;
}

query_node *
qstr_node(char *str) {
  query_node *node = query_node_new(STR);
  node->val = qc2pstring(str);
  return node;
}

query_node *
expr_node() {
  return query_node_new(EXPR);
}

query_node *
clause_node(query_node *str, cmp_type cmp, query_node *val) {
  query_node *node = query_node_new(CLAUSE);
  node->children = str;
  str->next = val;
  node->cmp = cmp;
  return node;
}

query_node *
mstr_node() {
  return query_node_new(MODSTR);
}


%}

%token<cptr> AND OR NOT LPAREN RPAREN EQ GT LT GTE LTE EOQ
%token<cptr> TILDE NEQ MINUS DIGITS DOT STRING_LITERAL UNQUOTED

%type<node> number string full_expression expression clause 
%type<node> simple_clause value modified_string 
%type<num> binary_expression_operator 
%type<cmp> comparison_operator

%start full_expression

%%

number
	: DIGITS            { $$ = int_node(context->ptr); }
	| MINUS DIGITS      { $$ = int_node(context->ptr); $$->num *= -1; }
	;
	
string
  : STRING_LITERAL    { $$ = qstr_node(context->ptr); }
  | UNQUOTED          { $$ = str_node(context->ptr); }
  ;
	
full_expression
  : expression EOQ  { query_parser_construct(context, $$); }
  ;

expression
  : clause                                          { $$ = expr_node(); $$->children = $1; }
  | clause binary_expression_operator expression    { $$ = $3; $1->next = $3->children; $3->children = $1; $1->logic *= $2; }
  | LPAREN expression RPAREN                        { $$ = $2; }
  ;

clause
  : simple_clause             { $$ = $1; $1->logic = 1; }
  | NOT simple_clause         { $$ = $2; $2->logic = -1; }
  ;


simple_clause
  : string comparison_operator value  { $$ = clause_node($1, $2, $3); }
  ;
  
comparison_operator
  : EQ      { $$ = _EQ     ;}
  | GT      { $$ = _GT     ;}
  | LT      { $$ = _LT     ;}
  | GTE     { $$ = _GTE    ;}
  | LTE     { $$ = _LTE    ;}
  | TILDE   { $$ = _TILDE  ;}
  | NEQ     { $$ = _NEQ    ;}
  ;


value
  : modified_string   
  | number
  ;


modified_string
  : string LPAREN modified_string RPAREN  { $$ = mstr_node(); $$->children = $1; $1->next = $3; }
  | string             
  ;


binary_expression_operator
  : AND                 { $$ = 1; }
  | OR                  { $$ = 2; }
  ;
   
%%
