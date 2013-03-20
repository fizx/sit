#ifndef SYSLOG_PARSER_H_INCLUDED
#define SYSLOG_PARSER_H_INCLUDED

#include "parser.h"

Parser *
syslog_simple_parser_new();

void
syslog_parser_free(void *parser);

Parser *
syslog_fresh_copy(Parser *parser);

#endif

