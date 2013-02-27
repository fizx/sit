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
#include "callback.h"
#include "pstring.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// extern int yydebug;
// yydebug = 1;

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

ast_node_t *
query_node_copy_subtree(query_parser *context, ast_node_t *subtree) {
  if(!subtree) return NULL;
  ast_node_t *cp = query_node_new(context, Q(subtree)->type);
  Q(cp)->num = Q(subtree)->num;
  Q(cp)->cmp = Q(subtree)->cmp;
  Q(cp)->negated = Q(subtree)->negated;
  if(Q(subtree)->val) Q(cp)->val = pcpy(Q(subtree)->val);
  if(Q(subtree)->field) Q(cp)->field = pcpy(Q(subtree)->field);
  
  
  cp->next = query_node_copy_subtree(context, subtree->next);
  if(cp->next) cp->next->prev = cp;
  cp->child = query_node_copy_subtree(context, subtree->child);
  ast_node_t *child = cp->child;
  while(child) {
    child->parent = cp;
    child = child->next;
  }
  return cp;
}


%}

%token<cptr> AND OR NOT LPAREN RPAREN EQ GT LT GTE LTE EOQ
%token<cptr> TILDE NEQ MINUS DIGITS DOT STRING_LITERAL UNQUOTED LIMIT

%type<node> number string full_expressions full_expression expression 
%type<node> value modified_string clause 
%type<node> comparison_operator
%type<num> expression_modifiers
%expect 0
%start full_expressions

%left OR
%left AND
%right NOT

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
  : expression expression_modifiers EOQ  { 
    query_parser_construct(context, $1, $2); 
  }
  | expression EOQ  { 
    query_parser_construct(context, $1, -1); 
  }
  ;

expression_modifiers 
  : LIMIT number { $$ = Q($2)->num; }

expression
  : expression OR expression  { 
      $$ = expr_node(context);
      ast_node_prepend_child($$, $1);
      ast_node_insert_after($1, $3);
      ast_node_insert_after($1, query_node_new(context, BOR));
    }
  | expression AND expression  { 
      $$ = expr_node(context);
      ast_node_prepend_child($$, $1);
      ast_node_insert_after($1, $3);
      ast_node_insert_after($1, query_node_new(context, BAND));
    }
  | LPAREN expression RPAREN                          { 
      $$ = expr_node(context);
      ast_node_prepend_child($$, $2);
    }
  | NOT expression                                { 
      $$ = $2;
      Q($$)->negated = !Q($$)->negated;
    }
  | clause                                            { 
      $$ = expr_node(context);
      ast_node_prepend_child($$, $1);
    }
  ;

clause
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
   
%%
