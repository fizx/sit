#ifndef CALLBACK_RUBY_H_INCLUDED
#define CALLBACK_RUBY_H_INCLUDED

#include "ruby.h"

VALUE
rbc_callback_new(VALUE class, VALUE user_data, VALUE block);

VALUE
rbc_callback_call(VALUE self, VALUE sit_data);

VALUE
rbc_callback_to_s(VALUE self);

#endif
