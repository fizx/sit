#ifndef QUERY_RUBY_H_INCLUDED
#define QUERY_RUBY_H_INCLUDED

#include "ruby.h"

VALUE
rbc_query_new(VALUE class, VALUE conjunctions, VALUE callback);

VALUE 
rbc_query_to_s(VALUE self);

VALUE 
rbc_query_equals(VALUE self, VALUE other);

VALUE
rbc_conjunction_new(VALUE class, VALUE terms);

VALUE 
rbc_conjunction_to_s(VALUE self);

VALUE 
rbc_conjunction_equals(VALUE self, VALUE other);

#endif
