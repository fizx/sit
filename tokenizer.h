#ifndef TOKENIZER_H_INCLUDED
#define TOKENIZER_H_INCLUDED

#include "pstring.h"
#include "callback.h"

typedef struct Tokenizer {
	struct Tokenizer *(*fresh_copy)(struct Tokenizer *tok);
  void (*end_stream)(struct Tokenizer *tok);
	void (*consume)(struct Tokenizer *tok, pstring *str);
	void *state; // for internal parser state
	void *data;  // for user purposes, e.g. the ruby bindings keep the ruby object here.
	
  Callback *on_error;
  Callback *on_token;
} Tokenizer;

typedef struct Token {
  pstring *text;
  int      offset;
} Token;

Tokenizer *
tokenizer_new();

#endif

