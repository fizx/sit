%token_table
%debug
%locations
%defines
%error-verbose

%define api.pure full
%define api.push-pull both
%define api.prefix rx

%parse-param { RegexParserState* context }
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

int rxlex(RXSTYPE* lvalp, RXLTYPE* llocp, void* scanner);

void rxerror(RXLTYPE* locp, RegexParserState *parser, const char* err) {
  (void) locp;
  parser->error = c2pstring(err);
}

#define scanner context->scanner

void
_ret_inner(Callback *cb, void *data) {
  Token *token = data;
  DocBuf *buf = cb->user_data;
  doc_buf_term_found(buf, token->text, token->offset);
}

%}

%token<pstr> RWITH RAS RINT RTOKENIZED RLPAREN RRPAREN RSTRING_LITERAL RUNQUOTED REOQ RCOMMA
%type<pstr> expression regex modifier modifiers impl name
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

name
  : RUNQUOTED  { $$ = context->ptr; }
  
modifier
  : name RAS impl                      { 
    context->fields[context->count].name = $1;
    context->fields[context->count].type = $3 ? TOKENS : INT; 
    if($3) {
      char *c = p2cstring($3);
      context->fields[context->count].tokenizer = regex_tokenizer_new(c); 
      context->fields[context->count].tokenizer->on_token = callback_new(_ret_inner, context->parser->buffer);
      free(c);
    }
    context->count++;
  }
  ;

impl
  : RINT                                    { $$ = NULL; }
  | RTOKENIZED RLPAREN regex RRPAREN        { $$ = $3; }
  ;

%%
