#ifndef VSTRING_RUBY_H_INCLUDED
#define VSTRING_RUBY_H_INCLUDED

#include "sit.h"
#include "ruby.h"

VALUE
rbc_vstring_new(VALUE class);

VALUE
rbc_vstring_append(VALUE self, VALUE rstr);

VALUE 
rbc_vstring_get(VALUE self, VALUE roff, VALUE rlen);

VALUE
rbc_vstring_shift(VALUE self, VALUE roff);

VALUE
rbc_vstring_to_s(VALUE self);

VALUE
rbc_vstring_size(VALUE self);

VALUE
rbc_vstring_nodes(VALUE self);

#endif
