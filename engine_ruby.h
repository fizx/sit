#ifndef ENGINE_RUBY_H_INCLUDED
#define ENGINE_RUBY_H_INCLUDED

#include "ruby.h"

VALUE
rbc_engine_new(VALUE class, VALUE parser, VALUE size);

VALUE
rbc_engine_parser(VALUE self);

#endif
