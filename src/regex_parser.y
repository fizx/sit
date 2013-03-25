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
  #include "_regex_parser.h"
  #include "pstring.h"
  typedef struct ReToken {
    pstring *name;
    pstring *value;
    FieldType type;
  } ReToken;
}

%union
{ 
  ReToken token;
}

%{
#include "callback.h"
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

%token<token> RWITH RAS RINT RTOKENIZED RLPAREN RRPAREN RSTRING_LITERAL RUNQUOTED REOQ RCOMMA
%type<token> expression regex select selects impl name
%expect 0
%start expression

%%

expression
  : regex RWITH selects REOQ  { context->pattern = $1.name; YYACCEPT; }
  | regex REOQ                { context->pattern = $1.name; YYACCEPT; }
  ;
  
regex
  : RSTRING_LITERAL     { $$.name = context->ptr; }
  ;

selects 
  : select
  | selects RCOMMA select
  ;

name
  : RUNQUOTED  { $$.name = context->ptr; }
  
select
  : impl RAS name                      { 
    context->fields[context->count].name = $1.name;
    context->fields[context->count].type = $1.type;
    context->fields[context->count].alias = $3.name; 
    if($1.type == TOKENS) {
      char *c = p2cstring($1.value);
      context->fields[context->count].tokenizer = regex_tokenizer_new(c); 
      context->fields[context->count].tokenizer->on_token = callback_new(_ret_inner, context->parser->buffer);
      free(c);
    }
    context->count++;
  }
  | impl {
    context->fields[context->count].name = $1.name;
    context->fields[context->count].type = $1.type;
    context->fields[context->count].alias = $1.name;     
    if($1.type == TOKENS) {
      char *c = p2cstring($1.value);
      context->fields[context->count].tokenizer = regex_tokenizer_new(c); 
      context->fields[context->count].tokenizer->on_token = callback_new(_ret_inner, context->parser->buffer);
      free(c);
    }
    
    context->count++;
  }
  ;

impl
  : RINT RLPAREN name RRPAREN                    { $$.type = INT; $$.name = $3.name; $$.value = NULL; }
  | RTOKENIZED RLPAREN name RCOMMA regex RRPAREN { $$.type = TOKENS; $$.name = $3.name; $$.value = $5.name; }
  | name                                         { $$.type = STRING; $$.name = $1.name; }
  ;

%%
