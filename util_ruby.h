#ifndef UTIL_RUBY_H_INCLUDED
#define UTIL_RUBY_H_INCLUDED

#include "ruby.h"
#include <stdbool.h>

typedef struct value_holder {
	VALUE val;
  struct value_holder *next;
} value_holder;

const void *
vwrap(VALUE value);

VALUE
vunwrap(const void *vp);

void 
markall();

bool
isTestMode();

void
setTestMode(bool val);

VALUE
rbc_isTestMode();

VALUE
rbc_setTestMode(VALUE val);

#endif
