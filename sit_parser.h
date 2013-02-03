#ifndef SIT_PARSER_H_INCLUDED
#define SIT_PARSER_H_INCLUDED

#include "pstring.h"

typedef struct sit_parser {
	void (*consume)(struct sit_parser *parser, pstring *str);
  void (*term_found)(struct sit_parser *parser, long off, int len, int field_offset);
  void (*document_found)(struct sit_parser *parser, long off, int len);
  void (*field_found)(struct sit_parser *parser, pstring *name);
	void *data;
} sit_parser;

sit_parser *
sit_parser_new();

#endif

