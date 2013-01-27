#include "sit_parser.h"
#include <stdlib.h>

sit_parser *
sit_parser_new() {
	sit_parser *parser = malloc(sizeof(sit_parser));
	parser->consume = NULL;
	parser->data = NULL;
	return parser;
}
