#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

#include "pstring.h"
#include "callback.h"
#include "doc_buf.h"

typedef struct Parser {
	struct Parser *(*fresh_copy)(struct Parser *parser);
  void (*set_offset)(struct Parser *parser, long off);
  void (*end_stream)(struct Parser *parser);
	void (*consume)(struct Parser *parser, pstring *str);
	void *state; // for internal parser state
	void *data;  // for user purposes, e.g. the ruby bindings keep the ruby object here.
  void (*free)(void *parser);
	
  DocBuf *buffer;
	
  Callback *on_error;
  Callback *on_document;
} Parser;

Parser *
parser_new();

void
parser_free(Parser *parser);

Parser *
default_fresh_copy(Parser *parser);

#endif

