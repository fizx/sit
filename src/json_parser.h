#ifndef JSON_PARSER_H_INCLUDED
#define JSON_PARSER_H_INCLUDED

#include "parser.h"
#include "engine.h"
#include "callback.h"
#include "tokenizer.h"
#include "white_tokenizer.h"

Parser *
json_white_parser_new();

Parser *
json_parser_new(Tokenizer *tokenizer);

Parser *
json_fresh_copy(Parser *parser);

#endif
