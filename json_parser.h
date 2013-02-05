#ifndef JSON_PARSER_H_INCLUDED
#define JSON_PARSER_H_INCLUDED

#include "sit_parser.h"
#include "sit_engine.h"
#include "sit_callback.h"

sit_parser *
json_white_parser_new(sit_engine *engine);

sit_parser *
json_parser_new(sit_engine *engine, sit_parser *tokenizer);

#endif
