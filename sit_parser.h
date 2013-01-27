#ifndef SIT_PARSER_H_INCLUDED
#define SIT_PARSER_H_INCLUDED

#include "pstring.h"

typedef struct {
	void (*consume)(void *parser, pstring *str);
	void *data;
} sit_parser;

sit_parser *
sit_parser_new();

#endif

