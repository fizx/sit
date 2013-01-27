#include "sit_parser.h"
#include <stdlib.h>

sit_parser *
sit_parser_new() {
	return malloc(sizeof(sit_parser));
}
