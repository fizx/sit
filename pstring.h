#ifndef PSTRING_H_INCLUDED
#define PSTRING_H_INCLUDED

#include <stdarg.h>

typedef struct {
  const char   *val;
  int           len;
} pstring;

#define P(x)   padd(buf, x)
#define PC(x)   paddc(buf, x)
#define PV(x, ...)   paddv(buf, x, ##__VA_ARGS__)

void
paddc(pstring *pstr, const char *cstr);

char *
pstring_cslice(pstring *pstr, long off, int len);

void
paddv(pstring *pstr, const char *fmt, ...);

void
padd(pstring *pstr, pstring *append);

pstring *
pstring_new(int len);

pstring *
pstring_new2(const char *cstr, int len);

pstring *
pstringf(const char *fmt, ...);

pstring *
pcpy(const pstring *pstr);

const char *
p2cstring(pstring *pstr);

char *
cstring_new(const char *cstr, int len);

pstring *
c2pstring(const char *cstr);

pstring *
qc2pstring(const char *cstr);

void
pstring_free(pstring *pstr);

void
pstring_copulate(pstring *target, pstring *pstr);

int
pstrcmp(pstring *a, pstring *b);

int
cpstrcmp(const char *a, pstring *b);

unsigned int
pstrhash(pstring *pstr);

char * 
pstrnchr(pstring *base, char *start, const char c);

char * 
pstrnstr(pstring *base, char *start, const char *c);

#endif
