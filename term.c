#include "sit.h"

Term *
term_new(pstring *field, pstring *text, int offset, bool negated) {
 	Term *term = malloc(sizeof(Term));
	term->field = *field;
	term->text = *text;
	term->offset = offset;
  term->negated = negated;
  term->numeric = false;
	term_update_hash(term);
	return term;
}

void 
term_free(Term *term) {
	if(term->owns_string) {
		pstring_free(&term->field);
		pstring_free(&term->text);
	}  
	free(term);
}

Term *
term_copy(const Term *term) {	
 	Term *copy = malloc(sizeof(Term));
  copy->field.val = malloc(term->field.len);
  memcpy((void*)copy->field.val, term->field.val, term->field.len);
  copy->field.len = term->field.len;
  copy->text.val = malloc(term->text.len);
  memcpy((void*)copy->text.val, term->text.val, term->text.len);
  copy->text.len = term->text.len;
  copy->owns_string = true; 	  
  copy->offset = term->offset;
  copy->negated = term->negated;
  copy->numeric = term->numeric;
  copy->hash_code = term->hash_code;
  return copy;
}

pstring *
term_to_s(Term *term) {	
	pstring *buf = pstring_new(0);
	PV("%.*s:%.*s", term->field.len, term->field.val, 
	term->text.len, term->text.val);
	return buf;
}

int
termcmp(const void *a, const void *b) {
	Term *ta = (Term *) a;
	Term *tb = (Term *) b;
	
  // int out = ta->negated - tb->negated;
  int out = 0;
  
	if (!out) {
	  out = pstrcmp(&ta->field, &tb->field);
  }
	if (!out) {
		out = pstrcmp(&ta->text, &tb->text);
	}
	return out;
}

void 
term_update_hash(Term *term) {
	term->hash_code = pstrhash(&term->field);
	term->hash_code ^= pstrhash(&term->text);
}
