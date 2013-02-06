#include "sit_parser.h"
#include "sit_engine.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void 
_default_term_found(struct sit_parser *parser, long off, int len, int field_offset) {
	assert(parser->forwards_to);
  sit_engine_term_found(parser->forwards_to, off, len, field_offset);
}

void 
_default_document_found(struct sit_parser *parser, long off, int len) {
	assert(parser->forwards_to);
  sit_engine_document_found(parser->forwards_to, off, len);
}

void 
_default_field_found(struct sit_parser *parser, pstring *name) {
	assert(parser->forwards_to);
  sit_engine_field_found(parser->forwards_to, name);
}

void 
_default_error_found(struct sit_parser *parser, pstring *message) {
	(void) parser;
  printf("%.*s\n", message->len, message->val);
}

sit_parser *
sit_parser_new() {
	sit_parser *parser = malloc(sizeof(sit_parser));
	parser->consume = NULL;
  parser->term_found = _default_term_found;
  parser->document_found = _default_document_found;
  parser->field_found = _default_field_found;
  parser->error_found = _default_error_found;
	return parser;
}