%option reentrant
%option bison-bridge
%option bison-locations
%option noyywrap
%option yylineno
%option extra-type="RegexParser*"

D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
NS    [^ \t\n\r,?*();:\^"]

%{

#include "sit.h"
#define YYSTYPE         RESTYPE
#define YYLTYPE         RELTYPE
/* Substitute the variable and function names.  */
#define yyparse         reparse
#define yypush_parse    repush_parse
#define yypull_parse    repull_parse
#define yypstate_new    repstate_new
#define yypstate_delete repstate_delete
#define yypstate        repstate
#define yylex           relex
#define yyerror         reerror
#define yylval          relval
#define yychar          rechar
#define yydebug         redebug
#define yynerrs         renerrs
#define yylloc          relloc

#define YY_USER_ACTION yylloc->first_line = yylineno;

#define YY_INPUT(cbuf, offset, max_size)  {                                 \
  int i;        puts("fuck");                                                            \
  for(i = 0; i < max_size && offset < yyextra->buf->len; i++, offset++) {   \
    cbuf[i] = yyextra->buf->val[offset];                                    \
  }                                                                         \
  if(i == 0) {                                                              \
    yyextra->done++;                                                        \
  }                                                                         \
  offset = i;                                                               \
}

%}

%%

("with"|"WITH")             { return(RWITH); }
("as"|"AS")                 { return(RAS); }
("int"|"INT")               { return(RINT); }
("tokenized"|"TOKENIZED")   { return(RTOKENIZED); }
"("                         { return(RLPAREN); }
")"                         { return(RPAREN); }
","                         { return(RCOMMA); }
L?`(\\.|[^`])*`      	      { yyextra->ptr = pstring_new2(yytext, yyleng); ll_add(&yyextra->freeable, yyextra->ptr); return(RSTRING_LITERAL); }
{NS}+             	        { yyextra->ptr = pstring_new2(yytext, yyleng); ll_add(&yyextra->freeable, yyextra->ptr); return(RUNQUOTED); }
[;\n]+                      { return(REOQ); }
[ \t\v\f]  		              { /* ignore whitespace*/ }
.			                      { /* ignore bad characters */ }

%%

RegexParser *
regex_parser_new(pstring *query) {
  RegexParser *parser = malloc(sizeof(RegexParser));
  relex_init(&parser->scanner);
  puts("WAT");
  parser->lvalp = malloc(sizeof(RESTYPE));
  parser->llocp = malloc(sizeof(RELTYPE));
  reset_extra(parser, parser->scanner);
  
  parser->buf = query;
  reparse(parser);
  return parser;
}

RegexParser *
regex_parser_fresh_copy(RegexParser *parser) {
  return regex_parser_new(parser->buf);
}

