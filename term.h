#ifndef SIT_TERM_H_INCLUDED
#define SIT_TERM_H_INCLUDED

#include "pstring.h"
#include <stdbool.h>

/**
 * A term is a key:value pair, with some limited metadata.  It's used by 
 * a bunch of different things, so the meanings are a little overloaded.
 * The quirkiest thing right now is that points > 1 is a term with
 * field:points, offset:1, numeric:true, text:>.
 */
typedef struct sit_term {
  pstring *field;
  pstring *text;
  unsigned int hash_code;
  unsigned int offset;
  bool negated;
  bool numeric;
} sit_term;

sit_term *
sit_term_new(pstring *field, pstring *text, int offset, bool negated);

pstring *
sit_term_to_s(sit_term *term);

sit_term *
sit_term_copy(sit_term *term);

void 
sit_term_free(sit_term *term);

void 
sit_term_update_hash(sit_term *term);

int
sit_termcmp(const void *a, const void *b);

#endif
