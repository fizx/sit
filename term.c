#include "sit.h"

sit_term *
sit_term_new(pstring *field, pstring *text, int offset, bool negated) {
 	sit_term *term = malloc(sizeof(sit_term));
	term->field = field;
	term->text = text;
	term->offset = offset;
  term->negated = negated;
  term->numeric = false;
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

sit_term *
sit_term_copy(sit_term *term) {	
 	sit_term *copy = malloc(sizeof(sit_term));
  copy->field = pcpy(term->field);
  copy->text = pcpy(term->text);
  copy->offset = term->offset;
  copy->negated = term->negated;
	sit_term_update_hash(copy);
  return copy;
}

pstring *
sit_term_to_s(sit_term *term) {	
	assert(term->field);
	assert(term->text);
	pstring *buf = pstring_new(0);
	PV("%.*s:%.*s", term->field->len, term->field->val, 
	term->text->len, term->text->val);
	return buf;
}

int
sit_termcmp(const void *a, const void *b) {
	sit_term *ta = (sit_term *) a;
	sit_term *tb = (sit_term *) b;
	
  // int out = ta->negated - tb->negated;
  int out = 0;
  
	if (!out) {
	  out = pstrcmp(ta->field, tb->field);
  }
	if (!out) {
		out = pstrcmp(ta->text, tb->text);
	}
	return out;
}

void 
sit_term_update_hash(sit_term *term) {
	assert(term->field);
	assert(term->text);
	term->hash_code = pstrhash(term->field);
	term->hash_code ^= pstrhash(term->text);
}
