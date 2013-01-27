#include <stdlib.h>
#include "sit_term.h"
#include "dict.h"

sit_term *
sit_term_new(pstring *field, pstring *text, int offset) {
 	sit_term *term = malloc(sizeof(sit_term));
	term->field = field;
	term->text = text;
	term->offset = offset;
	sit_term_update_hash(term);
	return term;
}

void 
sit_term_free(sit_term *term) {
	if(term != NULL) {
		pstring_free(term->field);
		pstring_free(term->text);
		free(term);
	}
}

int
sit_termcmp(const void *a, const void *b) {
	sit_term *ta = (sit_term *) a;
	sit_term *tb = (sit_term *) b;
	int out = pstrcmp(ta->field, tb->field);
	if (out == 0) {
		out = pstrcmp(ta->text, tb->text);
	}
	return out;
}


void 
sit_term_update_hash(sit_term *term) {
	term->hash_code = dictGenHashFunction(term->field->val, term->field->len);
	term->hash_code ^= dictGenHashFunction(term->text->val, term->text->len);
}
