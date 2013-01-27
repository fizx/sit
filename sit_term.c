#include <stdlib.h>
#include "sit_term.h"

sit_term *
sit_term_new() {
 	return malloc(sizeof(sit_term));
}

void 
sit_term_free(sit_term *term) {
	if(term != NULL) {
		pstring_free(term->field);
		pstring_free(term->text);
		free(term);
	}
}