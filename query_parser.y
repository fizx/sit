
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
  ast_node_t *node;
  int         num;
  char       *cptr;
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
#define Q(node)   ((query_node *)(node->data))

ast_node_t *
int_node(query_parser *context) {
  ast_node_t *node = query_node_new(context, NUM);
  Q(node)->num = atoi(context->ptr);
  return node;
}

ast_node_t *
str_node(query_parser *context) {
  ast_node_t *node = query_node_new(context, STR);
  Q(node)->val = c2pstring(context->ptr);
  return node;
}

ast_node_t *
qstr_node(query_parser *context) {
  ast_node_t *node = query_node_new(context, STR);
  Q(node)->val = qc2pstring(context->ptr);
  return node;
}

ast_node_t *
expr_node(query_parser *context) {
  return query_node_new(context, EXPR);
}

ast_node_t *
clause_node(query_parser *context) {
  return query_node_new(context, CLAUSE);
}

ast_node_t *
mstr_node(query_parser *context) {
  return query_node_new(context, MODSTR);
}

ast_node_t *
cmp_node(query_parser *context, cmp_type c) {
  ast_node_t *node = query_node_new(context, CMP);
  Q(node)->cmp = c;
  return node;
}


%}

%token<cptr> AND OR NOT LPAREN RPAREN EQ GT LT GTE LTE EOQ
%token<cptr> TILDE NEQ MINUS DIGITS DOT STRING_LITERAL UNQUOTED

%type<node> number string full_expressions full_expression expression 
%type<node> simple_clause value modified_string clause 
%type<node> binary_expression_operator comparison_operator
%expect 6
%start full_expressions

%%

number
	: DIGITS            { $$ = int_node(context); }
	| MINUS DIGITS      { $$ = int_node(context); Q($$)->num *= -1; }
	;
	
string
  : STRING_LITERAL    { $$ = qstr_node(context); }
  | UNQUOTED          { $$ = str_node(context); }
  ;
  
full_expressions
  : full_expression full_expressions
  | full_expression
  ;

full_expression
  : expression EOQ  { query_parser_construct(context, $$); }
  ;

expression
  : clause binary_expression_operator expression      { 
      $$ = $3;
      ast_node_prepend_child($3, $1);
      ast_node_insert_after($1, $2);
    }
  | expression binary_expression_operator expression  { 
      $$ = expr_node(context);
      ast_node_prepend_child($$, $1);
      ast_node_insert_after($1, $2);
      ast_node_insert_after($2, $3);
    }
  | LPAREN expression RPAREN                          { $$ = $2; }
  | clause                                            { 
      $$ = expr_node(context);
      ast_node_prepend_child($$, $1);
    }
  ;

clause
  : simple_clause             { $$ = $1; Q($1)->negated = false; }
  | NOT simple_clause         { $$ = $2; Q($2)->negated = true; }
  ;


simple_clause
  : string comparison_operator value  { 
      $$ = clause_node(context);
      ast_node_prepend_child($$, $1);
      ast_node_insert_after($1, $2);
      ast_node_insert_after($2, $3);
    }
  ;
  
comparison_operator
  : EQ      { $$ = cmp_node(context, _EQ     );}
  | GT      { $$ = cmp_node(context, _GT     );}
  | LT      { $$ = cmp_node(context, _LT     );}
  | GTE     { $$ = cmp_node(context, _GTE    );}
  | LTE     { $$ = cmp_node(context, _LTE    );}
  | TILDE   { $$ = cmp_node(context, _TILDE  );}
  | NEQ     { $$ = cmp_node(context, _NEQ    );}
  ;


value
  : modified_string   
  | number
  ;


modified_string
  : string LPAREN modified_string RPAREN  { 
      $$ = $1; 
      Q($1)->type = MODSTR; 
      ast_node_insert_after($1, $3);
    }
  | string             
  ;


binary_expression_operator
  : AND                 { $$ = query_node_new(context, BAND); }
  | OR                  { $$ = query_node_new(context, BOR); }
  ;
   
%%
