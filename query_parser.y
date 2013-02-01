
%token_table
%debug
%locations
%defines
%error-verbose

%define api.pure full
%define api.push-pull push

%parse-param { query_parser* context }
%lex-param { void* scanner  }

%code requires {
  #include "query_parser.h"
}

%union
{
   int integer;
   char* cptr;
}


%{

#include "query_parser.h"
#include "sit_callback.h"
#include "pstring.h"
#include <stdlib.h>
#include <stdio.h>

int yylex(YYSTYPE* lvalp, YYLTYPE* llocp, void* scanner);

void yyerror(YYLTYPE* locp, query_parser *parser, const char* err) {
  (void) locp;
  parser->error = c2pstring(err);
}

#define scanner context->scanner

%}

%token<cptr> AND OR NOT LPAREN RPAREN EQ GT LT GTE LTE BRK
%token<cptr> TILDE NEQ MINUS DIGITS DOT STRING_LITERAL UNQUOTED

%start full_expression

%%

number
	: DIGITS
	| DIGITS DOT DIGITS
	| MINUS DIGITS
	| MINUS DIGITS DOT DIGITS
	;
	
string
  : STRING_LITERAL
  | UNQUOTED
  ;
	
full_expression
  : expression BRK

expression
  : clause
  | clause binary_expression_operator expression
  | LPAREN expression RPAREN
  ;

clause
  : simple_clause
  | NOT simple_clause
  ;


simple_clause
  : string comparison_operator value
  ;
  
comparison_operator
  : EQ
  | GT
  | LT
  | GTE
  | LTE
  | TILDE
  | NEQ
  ;


value
  : modified_string
  | number
  ;


modified_string
  : string LPAREN modified_string RPAREN
  | string
  ;


binary_expression_operator
  : AND
  | OR
  ;
   
%%
