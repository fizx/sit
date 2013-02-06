#ifndef SIT_TERM_H_INCLUDED
#define SIT_TERM_H_INCLUDED

#include "pstring.h"
#include <stdbool.h>

typedef struct {
  pstring *field;
  pstring *text;
  unsigned int hash_code;
  unsigned int offset;
  bool negated;
} sit_term;

sit_term *
sit_term_new(pstring *field, pstring *text, int offset, bool negated);

pstring *
sit_term_to_s(sit_term *term);

void 
sit_term_free(sit_term *term);

void 
sit_term_update_hash(sit_term *term);

int
sit_termcmp(const void *a, const void *b);

#endif
