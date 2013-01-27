#ifndef UTIL_RUBY_H_INCLUDED
#define UTIL_RUBY_H_INCLUDED

#include "ruby.h"

typedef struct {
	VALUE val;
} value_holder;

void *
vwrap(VALUE value);

VALUE
vunwrap(void *vp);

#endif
