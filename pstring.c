#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "pstring.h"
#include "dict.h"

pstring *
pstring_new(int len) {
  assert(len >= 0);
	pstring *pstr = malloc(sizeof(pstring));
	pstr->val = malloc(len);
	pstr->len = len;
	return pstr;
}

pstring *
c2pstring(const char *cstr) {
  pstring *pstr = pstring_new(strlen(cstr));
  strncpy(pstr->val, cstr, pstr->len);
  return pstr;
}

char *
cstring_new(char *cstr, int len) {
  char *new = malloc(len + 1);
  memcpy(new, cstr, len);
  new[len] = 0;
  return new;
}

pstring *
pcpy(pstring *pstr) {
  pstring *cpy = pstring_new(pstr->len);
  memcpy(cpy->val, pstr->val, pstr->len);
  return cpy;
}

void
padd(pstring *pstr, pstring *append) {
  int nlen = pstr->len + append->len;
  char *nbuf = malloc(nlen);
  memcpy(nbuf, pstr->val, pstr->len);
  memcpy(nbuf + pstr->len, append->val, append->len);
  free(pstr->val);
  pstr->val = nbuf;
  pstr->len = nlen;
}

void
paddc(pstring *pstr, char *cstr) {
  pstring tmp = {
    cstr,
    strlen(cstr)
  };
  padd(pstr, &tmp);
}

pstring *
pstring_new2(char *cstr, int len) {
  pstring *pstr = pstring_new(len);
  memcpy(pstr->val, cstr, len);
  return pstr;
}

pstring *
qc2pstring(const char *cstr) {
  pstring *pstr = pstring_new(strlen(cstr));
  char *val = pstr->val;
  int len = strlen(cstr);
  for(int i = 1; i < len - 1; i++) {
    if(cstr[i] == '\\') {
      i++;
    } else {
      *val++ = cstr[i];
    }
  }
  pstr->len = val - pstr->val;
  return pstr;
}

void
pstring_free(pstring *pstr) {
	if(pstr != NULL) {
		if(pstr->val != NULL) {
			free(pstr->val);
		}
		free(pstr);
	}
}

int
pstrcmp(pstring *a, pstring *b) {
  if(a == b) return 0;
  if(a == NULL) return -1;
  if(b == NULL) return 1;
	int min = a->len > b->len ? b->len : a->len;
	int out = strncmp(a->val, b->val, min);
	if (!out) {
		return a->len - b->len;
	}
	return out;
}

unsigned int
pstrhash(pstring *pstr) {
  return dictGenHashFunction(pstr->val, pstr->len);
}