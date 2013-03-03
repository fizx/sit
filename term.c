#include "sit.h"

Term *
term_new(pstring *field, pstring *text, int offset, bool negated) {
 	Term *term = malloc(sizeof(Term));
	term->field = field;
	term->text = text;
	term->offset = offset;
  term->negated = negated;
  term->numeric = false;
	term_update_hash(term);
	return term;
}

void 
term_free(Term *term) {
  DEBUG("term_free");
	if(term != NULL) {
		pstring_free(term->field);
		pstring_free(term->text);
		free(term);
	}
}

Term *
term_copy(Term *term) {	
 	Term *copy = malloc(sizeof(Term));
  copy->field = pcpy(term->field);
  copy->text = pcpy(term->text);
  copy->offset = term->offset;
  copy->negated = term->negated;
  copy->numeric = term->numeric;
	term_update_hash(copy);
  assert(term->hash_code == copy->hash_code);
  return copy;
}

pstring *
term_to_s(Term *term) {	
	assert(term->field);
	assert(term->text);
	pstring *buf = pstring_new(0);
	PV("%.*s:%.*s", term->field->len, term->field->val, 
	term->text->len, term->text->val);
	return buf;
}

int
termcmp(const void *a, const void *b) {
	Term *ta = (Term *) a;
	Term *tb = (Term *) b;
	
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
term_update_hash(Term *term) {
	assert(term->field);
	assert(term->text);
	term->hash_code = pstrhash(term->field);
	term->hash_code ^= pstrhash(term->text);
}
