#include "parser.h"
#include "engine.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "util.h"

sit_parser *
default_fresh_copy(sit_parser *parser) {
	sit_parser *copy = sit_parser_new();
	copy->consume = parser->consume;
	SET_ONCE(copy->data, parser->data);
	SET_ONCE(copy->receiver, parser->receiver);
	return copy;
}

sit_parser *
sit_parser_new() {
	sit_parser *parser = calloc(1, sizeof(sit_parser));
	parser->consume = NULL;
	parser->fresh_copy = default_fresh_copy;
	return parser;
}