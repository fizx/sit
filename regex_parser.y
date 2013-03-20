%token_table
%debug
%locations
%defines
%error-verbose

%define api.pure full
%define api.push-pull both
%define api.prefix re

%parse-param { RegexParser* context }
%lex-param { void* scanner  }

%code requires {
  #include "regex_parser.h"
}

%union
{ 
  pstring *pstr;
}

%{
#include "query_parser.h"
#include "callback.h"
#include "pstring.h"
#include "util.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

int relex(RESTYPE* lvalp, RELTYPE* llocp, void* scanner);

void reerror(RELTYPE* locp, RegexParser *parser, const char* err) {
  (void) locp;
  parser->error = c2pstring(err);
}

#define scanner context->scanner

// c helpers

%}

%token<pstr> RWITH RAS RINT RTOKENIZED RLPAREN RRPAREN RSTRING_LITERAL RUNQUOTED REOQ RCOMMA
%type<pstr> expression regex modifier modifiers impl
%expect 0
%start expression

%%

expression
  : regex RWITH modifiers REOQ  { context->pattern = $1; YYACCEPT; }
  | regex REOQ                  { context->pattern = $1; YYACCEPT; }
  ;
  
regex
  : RSTRING_LITERAL     { $$ = context->ptr; }
  ;

modifiers 
  : modifier
  | modifiers RCOMMA modifier
  ;
  
modifier
  : RUNQUOTED RAS impl                      { 
    context->fields[context->count].name = $1;
    context->fields[context->count].type = $3;
    context->count++;
  }
  ;

impl
  : RINT                                    { $$ = NULL; }
  | RTOKENIZED RLPAREN regex RRPAREN        { $$ = $3; }
  ;

%%
