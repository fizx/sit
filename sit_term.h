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
sit_term_new();

void 
sit_term_free(sit_term *term);

#endif