#ifndef UTIL_RUBY_H_INCLUDED
#define UTIL_RUBY_H_INCLUDED

#include "ruby.h"

typedef struct {
	VALUE val;
} value_holder;

const void *
vwrap(VALUE value);

VALUE
vunwrap(const void *vp);

#endif
