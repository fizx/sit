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

int relex(RESTYPE* lvalp, RELTYPE* llocp, void* scanner);

void reerror(RELTYPE* locp, RegexParser *parser, const char* err) {
  (void) locp;
  parser->error = c2pstring(err);
}

#define scanner context->scanner

// c helpers

%}

%token<cptr> RWITH RAS RINT RTOKENIZED RLPAREN RRPAREN RSTRING_LITERAL RUNQUOTED REOQ RCOMMA

%type<cptr> expression regex modifier modifiers impl
%expect 0
%start expression

%%

expression
  : regex RWITH modifiers REOQ  
  | regex REOQ  
  ;
  
regex
  : RSTRING_LITERAL
  ;

modifiers 
  : modifier
  | modifiers RCOMMA modifier
  ;
  
modifier
  : RUNQUOTED RAS impl
  ;

impl
  : RINT
  | RTOKENIZED RLPAREN regex RRPAREN
  ;

%%
