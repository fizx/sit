D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
NS    [^ \t\n\r,?*():\^"]

%{
#include <stdio.h>
#include "y.tab.h"

void count();
%}

%option bison-bridge

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
{D}+                      { return(DIGITS); }
"."                       { return(DOT); }
L?\"(\\.|[^\\"])*\"	      { return(STRING_LITERAL); }
{NS}+             	      { return(UNQUOTED); }
[;\n]                     { return(BRK); }
[ \t\v\f]  		            { /* ignore whitespace*/ }
.			                    { /* ignore bad characters */ }

%%