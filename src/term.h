#ifndef SIT_TERM_H_INCLUDED
#define SIT_TERM_H_INCLUDED

#include "pstring.h"
#include <stdbool.h>

typedef enum { TEXT, NUMERIC, CATCHALL } TermType;

/**
 * A term is a key:value pair, with some limited metadata.  It's used by 
 * a bunch of different things, so the meanings are a little overloaded.
 * The quirkiest thing right now is that points > 1 is a term with
 * field:points, offset:1, numeric:true, text:>.
 */
typedef struct Term { 
  pstring field;
  pstring text;
  unsigned int hash_code;
  unsigned int offset;
  bool negated;
  TermType type;
  bool owns_string;
  struct Term *prev;
  struct Term *next;
} Term;

Term *
term_new(pstring *field, pstring *text, int offset, bool negated);

pstring *
term_to_s(Term *term);

Term *
term_copy(const Term *term);

void
term_copulate(Term *copy, const Term *term);

void 
term_free(Term *term);

void 
term_update_hash(Term *term);

int
termcmp(const void *a, const void *b);

#endif
