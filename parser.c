#include "sit.h"

Parser *
default_fresh_copy(Parser *parser) {
	Parser *copy = parser_new();
	copy->consume = parser->consume;
	SET_ONCE(copy->data, parser->data);
	SET_ONCE(copy->on_document, parser->on_document);
	SET_ONCE(copy->on_error, parser->on_error);
	return copy;
}

Parser *
parser_new() {
	Parser *parser = calloc(1, sizeof(Parser));
	parser->consume = NULL;
	parser->fresh_copy = default_fresh_copy;
	return parser;
}

void
parser_free(Parser *parser) {
  if(parser->free) {
    parser->free(parser);
  } else {
    free(parser);
  }
}