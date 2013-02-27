#include "sit.h"

Parser *
default_fresh_copy(Parser *parser) {
	Parser *copy = sit_parser_new();
	copy->consume = parser->consume;
	SET_ONCE(copy->data, parser->data);
	SET_ONCE(copy->receiver, parser->receiver);
	return copy;
}

Parser *
sit_parser_new() {
	Parser *parser = calloc(1, sizeof(Parser));
	parser->consume = NULL;
	parser->fresh_copy = default_fresh_copy;
	return parser;
}