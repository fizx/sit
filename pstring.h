#ifndef PSTRING_H_INCLUDED
#define PSTRING_H_INCLUDED

typedef struct {
  char 			   *val;
  int           len;
} pstring;

pstring *
pstring_new(int len);

void
pstring_free(pstring *pstr);

#endif