#ifndef SIT_PARSER_H_INCLUDED
#define SIT_PARSER_H_INCLUDED

#include "pstring.h"
#include "sit_receiver.h"


typedef struct sit_parser {
	struct sit_parser *(*fresh_copy)(struct sit_parser *parser);
  void (*set_offset)(struct sit_parser *parser, long off);
  void (*end_stream)(struct sit_parser *parser);
	void (*consume)(struct sit_parser *parser, pstring *str);
	void *state; // for internal parser state
	sit_receiver *receiver;  // if this parser forwards events to another object (usually engine)
	void *data;  // for user purposes, e.g. the ruby bindings keep the ruby object here.
} sit_parser;

sit_parser *
sit_parser_new();


sit_parser *
default_fresh_copy(sit_parser *parser);

#endif

