#ifndef PSTRING_H_INCLUDED
#define PSTRING_H_INCLUDED

typedef struct {
  char 			   *val;
  int           len;
} pstring;

pstring *
pstring_new(int len);

pstring *
c2pstring(const char *cstr);

void
pstring_free(pstring *pstr);

int
pstrcmp(pstring *a, pstring *b);

unsigned int
pstrhash(pstring *pstr);

#endif
