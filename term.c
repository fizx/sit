#include "sit.h"

Term *
term_new(pstring *field, pstring *text, int offset, bool negated) {
 	Term *term = malloc(sizeof(Term));
 	const Term tmp = {
 	  *field,
 	  *text,
 	  0,
 	  offset,
 	  negated,
 	  false,
 	  false
  };
	term_update_hash(&tmp);
  term_copulate(term, &tmp);
	return term;
}

void 
term_free(Term *term) {
	if(term->owns_string) {
    free(term->field.val);
		free(term->text.val);
	}  
	free(term);
}

void
term_copulate(Term *copy, const Term *term) {	  
  pstring_copulate(&copy->field, &term->field);
  pstring_copulate(&copy->text, &term->text);
  copy->owns_string = true; 	  
  copy->offset = term->offset;
  copy->negated = term->negated;
  copy->type = term->type;
  copy->hash_code = term->hash_code;
}

Term *
term_copy(const Term *term) {	
 	Term *copy = malloc(sizeof(Term));
  term_copulate(copy, term);
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
	
  int out = tb->type - ta->type;
  
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
