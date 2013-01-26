#ifndef TERM_RUBY_H_INCLUDED
#define TERM_RUBY_H_INCLUDED

#include "ruby.h"

VALUE
rbc_term_new(VALUE class, VALUE field, VALUE text, VALUE offset);

VALUE
rbc_term_to_s(VALUE self);

#endif