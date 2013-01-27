#ifndef QUERY_RUBY_H_INCLUDED
#define QUERY_RUBY_H_INCLUDED

#include "ruby.h"

VALUE
rbc_query_new(VALUE class, VALUE terms, VALUE callback);

VALUE 
rbc_query_to_s(VALUE self);

#endif
