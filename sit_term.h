#ifndef SIT_TERM_H_INCLUDED
#define SIT_TERM_H_INCLUDED

#include "pstring.h"

typedef struct {
  pstring *field;
  pstring *text;
  unsigned int hash_code;
  unsigned int offset;
} sit_term;

sit_term *
sit_term_new(pstring *field, pstring *text, int offset);

void 
sit_term_free(sit_term *term);

void 
sit_term_update_hash(sit_term *term);

int
sit_termcmp(const void *a, const void *b);

#endif
