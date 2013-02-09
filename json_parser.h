#ifndef JSON_PARSER_H_INCLUDED
#define JSON_PARSER_H_INCLUDED

#include "sit_parser.h"
#include "sit_engine.h"
#include "sit_callback.h"

sit_parser *
json_white_parser_new();

sit_parser *
json_parser_new(sit_parser *tokenizer);

sit_parser *
json_fresh_copy(sit_parser *parser);
#endif
