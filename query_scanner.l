%option reentrant
%option bison-bridge
%option bison-locations
%option noyywrap
%option yylineno

D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
NS    [^ \t\n\r,?*():\^"]

%{
#include "y.tab.h"
#include <stdio.h>
#include "query_parser.h"

#define YY_EXTRA_TYPE query_parser*
#define YY_USER_ACTION yylloc->first_line = yylineno;

#define YY_INPUT(cbuf, result, max_size)  {       \
  if(yyextra->off < yyextra->buf->len) {          \
    cbuf[0] = yyextra->buf->val[yyextra->off++];  \
    result = 1;                                   \
  } else {                                        \
    result = YY_NULL;                             \
  }                                               \
}

int yyparse (query_parser* context);

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
{D}+                      { return(DIGITS); }
"."                       { return(DOT); }
L?\"(\\.|[^\\"])*\"	      { return(STRING_LITERAL); }
{NS}+             	      { return(UNQUOTED); }
[;\n]                     { return(BRK); }
[ \t\v\f]  		            { /* ignore whitespace*/ }
.			                    { /* ignore bad characters */ }

%%

query_parser *
query_parser_new(sit_callback *cb) {
  query_parser *parser = malloc(sizeof(query_parser));
  parser->buf = NULL;
  parser->off = 0;
  parser->cb = cb;
  yylex_init(&parser->scanner);
  yyset_extra(parser, parser->scanner);
  return parser;
}

void
query_parser_consume(query_parser *parser, pstring *pstr) {
  parser->buf = pstr;
  
//  yypush_parse (yypstate *ps, int pushed_char, YYSTYPE const *pushed_val, YYLTYPE *pushed_loc, query_parser* context);
  yyparse(parser);
}