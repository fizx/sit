#ifndef VSTRING_H_INCLUDED
#define VSTRING_H_INCLUDED

#include "pstring.h"

typedef struct LList {
  const void   *data;
  struct LList *next;
} LList;

typedef struct VStringNode {
  pstring             pstr;
  long                off;
  struct VStringNode *prev;
  LList              *strings;
} VStringNode;

typedef struct vstring {
  struct VStringNode *node;
  long off;
} vstring;

void
ll_add(LList **ll, const void *data);

void
ll_free(LList *ll);

vstring *
vstring_new();

void
vstring_append(vstring *vstr, pstring *pstr);

void
vstring_get(pstring *target, vstring *vstr, long off);

long
vstring_size(vstring *vstr);

void
vstring_shift(vstring *vstr, long off);

void
vstring_free(vstring *vstr);

#endif
