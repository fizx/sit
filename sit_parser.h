#ifndef SIT_PARSER_H_INCLUDED
#define SIT_PARSER_H_INCLUDED

#include "pstring.h"

typedef struct sit_parser {
  void (*set_offset)(struct sit_parser *parser, long off);
  void (*end_stream)(struct sit_parser *parser);
	void (*consume)(struct sit_parser *parser, pstring *str);
  void (*term_found)(struct sit_parser *parser, long off, int len, int field_offset);
  void (*document_found)(struct sit_parser *parser, long off, int len);
  void (*field_found)(struct sit_parser *parser, pstring *name);
  void (*error_found)(struct sit_parser *parser, pstring *message);
  void (*int_found)(struct sit_parser *parser, int val);
	void *state; // for internal parser state
	void *data;  // for user purposes, e.g. the ruby bindings keep the ruby object here.
	void *forwards_to;  // if this parser forwards events to another object (usually engine)
} sit_parser;

sit_parser *
sit_parser_new();

#endif

