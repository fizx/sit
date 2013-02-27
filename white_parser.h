#ifndef WHITE_PARSER_H_INCLUDED
#define WHITE_PARSER_H_INCLUDED

#include "parser.h"

Parser *
white_parser_new();

Parser *
white_fresh_copy(Parser *parser);


#endif
