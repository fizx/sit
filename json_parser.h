#ifndef JSON_PARSER_H_INCLUDED
#define JSON_PARSER_H_INCLUDED

#include "parser.h"
#include "engine.h"
#include "callback.h"

Parser *
json_white_parser_new();

Parser *
json_parser_new(Parser *tokenizer);

Parser *
json_fresh_copy(Parser *parser);
#endif
