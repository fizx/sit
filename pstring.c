#include <stdlib.h>
#include <string.h>
#include "pstring.h"
#include "dict.h"

pstring *
pstring_new(int len) {
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
pstring_new2(char *cstr, int len) {
  pstring *pstr = pstring_new(len + 1);
  memcpy(pstr->val, cstr, len);
  pstr->val[len] = 0;
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