%token_table
%debug
%locations
%defines
%error-verbose

%define api.pure full
%define api.push-pull both

%parse-param { QueryParser* context }
%lex-param { void* scanner  }

%code requires {
  #include "query_parser.h"
}

%union
{ 
  ASTNode *node;
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

void yyerror(YYLTYPE* locp, QueryParser *parser, const char* err) {
  (void) locp;
  parser->error = c2pstring(err);
}

#define scanner context->scanner
#define Q(node)   ((QNode *)(node->data))

ASTNode *
int_node(QueryParser *context) {
  ASTNode *node = ast_query_node_new(context, NUM);
  Q(node)->num = atoi(context->ptr);
  return node;
}

ASTNode *
str_node(QueryParser *context) {
  ASTNode *node = ast_query_node_new(context, STR);
  Q(node)->val = c2pstring(context->ptr);
  return node;
}

ASTNode *
qstr_node(QueryParser *context) {
  ASTNode *node = ast_query_node_new(context, STR);
  Q(node)->val = qc2pstring(context->ptr);
  return node;
}

ASTNode *
expr_node(QueryParser *context) {
  return ast_query_node_new(context, EXPR);
}

ASTNode *
clause_node(QueryParser *context) {
  return ast_query_node_new(context, CLAUSE);
}

ASTNode *
mstr_node(QueryParser *context) {
  return ast_query_node_new(context, MODSTR);
}

ASTNode *
cmp_node(QueryParser *context, CmpType c) {
  ASTNode *node = ast_query_node_new(context, CMP);
  Q(node)->cmp = c;
  return node;
}

ASTNode *
query_node_copy_subtree(QueryParser *context, ASTNode *subtree) {
  if(!subtree) return NULL;
  ASTNode *cp = ast_query_node_new(context, Q(subtree)->type);
  Q(cp)->num = Q(subtree)->num;
  Q(cp)->cmp = Q(subtree)->cmp;
  Q(cp)->negated = Q(subtree)->negated;
  if(Q(subtree)->val) Q(cp)->val = pcpy(Q(subtree)->val);
  if(Q(subtree)->field) Q(cp)->field = pcpy(Q(subtree)->field);
  
  
  cp->next = query_node_copy_subtree(context, subtree->next);
  if(cp->next) cp->next->prev = cp;
  cp->child = query_node_copy_subtree(context, subtree->child);
  ASTNode *child = cp->child;
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
      ast_node_insert_after($1, ast_query_node_new(context, BOR));
    }
  | expression AND expression  { 
      $$ = expr_node(context);
      ast_node_prepend_child($$, $1);
      ast_node_insert_after($1, $3);
      ast_node_insert_after($1, ast_query_node_new(context, BAND));
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
