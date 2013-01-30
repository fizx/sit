#ifndef CALLBACK_RUBY_H_INCLUDED
#define CALLBACK_RUBY_H_INCLUDED

#include "ruby.h"

VALUE
rbc_callback_new(VALUE class, VALUE rengine, VALUE block);

VALUE
rbc_callback_call(VALUE self);

VALUE
rbc_callback_to_s(VALUE self);

#endif
