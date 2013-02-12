#ifndef PSTRING_H_INCLUDED
#define PSTRING_H_INCLUDED

#include <stdarg.h>

typedef struct {
  const char 			   *val;
  int           len;
} pstring;

#define P(x)   padd(buf, x)
#define PC(x)   paddc(buf, x)

#define PV(x, ...)   paddv(buf, x, ##__VA_ARGS__)

pstring *
pstring_new(int len);

pstring *
pstring_new2(const char *cstr, int len);

void
padd(pstring *pstr, pstring *append);

pstring *
pcpy(pstring *pstr);

const char *
p2cstring(pstring *pstr);

void
paddc(pstring *pstr, const char *cstr);

void
paddv(pstring *pstr, const char *fmt, ...);

char *
cstring_new(const char *cstr, int len);

pstring *
c2pstring(const char *cstr);

pstring *
qc2pstring(const char *cstr);

void
pstring_free(pstring *pstr);

int
pstrcmp(pstring *a, pstring *b);

int
cpstrcmp(const char *a, pstring *b);

unsigned int
pstrhash(pstring *pstr);

#endif
