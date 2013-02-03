#ifndef PSTRING_H_INCLUDED
#define PSTRING_H_INCLUDED

typedef struct {
  char 			   *val;
  int           len;
} pstring;

pstring *
pstring_new(int len);

pstring *
pstring_new2(char *cstr, int len);

void
padd(pstring *pstr, pstring *append);

pstring *
pcpy(pstring *pstr);

void
paddc(pstring * pstr, char *cstr);

char *
cstring_new(char *cstr, int len);

pstring *
c2pstring(const char *cstr);

pstring *
qc2pstring(const char *cstr);

void
pstring_free(pstring *pstr);

int
pstrcmp(pstring *a, pstring *b);

unsigned int
pstrhash(pstring *pstr);

#endif
