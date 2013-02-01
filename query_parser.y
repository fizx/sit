%{

%}

// %define api.pure full
// %define api.push-pull push

%glr-parser
%token_table
%debug

%token AND OR NOT LPAREN RPAREN EQ GT LT GTE LTE BRK
%token TILDE NEQ MINUS DIGITS DOT STRING_LITERAL UNQUOTED

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
#include <stdio.h>

extern char yytext[];
extern int column;

yyerror(s)
char *s;
{
	fflush(stdout);
	printf("\n%*s\n%*s\n", column, "^", column, s);
}