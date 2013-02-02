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
#include "query_parser.h"
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
{D}+                      { yyextra->ptr = cstring_new(yytext, yyleng); printf("a '%s' %d\n", yyextra->ptr, yyleng); return(DIGITS); }
"."                       { printf(".DOT!\n");return(DOT); }
L?\"(\\.|[^\\"])*\"	      { yyextra->ptr = cstring_new(yytext, yyleng); printf("b '%s' %d\n", yyextra->ptr, yyleng); return(STRING_LITERAL); }
{NS}+             	      { yyextra->ptr = cstring_new(yytext, yyleng); printf("c '%s' %d\n", yyextra->ptr, yyleng); return(UNQUOTED); }
[;\n]+                    { printf("EQ\n"); return(EOQ); }
[ \t\v\f]  		            { /* ignore whitespace*/ }
.			                    { printf("wut"); /* ignore bad characters */ }

%%

query_parser *
query_parser_new(sit_callback *cb) {
  query_parser *parser = malloc(sizeof(query_parser));
  parser->buf = NULL;
  parser->error = NULL;
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
  printf("poff %d of %d\n", parser->done, pstr->len);
  return status;
}


void
query_parser_construct(query_parser *context, query_node *root) {
  assert(0);
};
