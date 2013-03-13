#ifndef VSTRING_H_INCLUDED
#define VSTRING_H_INCLUDED

#include <stdbool.h>

#include "pstring.h"
#include "ll.h"

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

vstring *
vstring_new();

void
vstring_append(vstring *vstr, pstring *pstr);

void
vstring_get(pstring *target, vstring *vstr, long off);

bool
vstring_gets(pstring *target, vstring *vstr);

long
vstring_size(vstring *vstr);

void
vstring_shift_retain(vstring *vstr, long off);

void
vstring_shift(vstring *vstr, long off);

void
vstring_free(vstring *vstr);

#endif
