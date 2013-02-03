#ifndef PSTRING_RUBY_H_INCLUDED
#define PSTRING_RUBY_H_INCLUDED

#include "pstring.h"
#include "ruby.h"

pstring *
r2pstring(VALUE rstr);

VALUE
p2rstring(pstring *pstr);

VALUE
rbc_pstring_new(VALUE class, VALUE rstr);

VALUE
rbc_pstring_to_s(VALUE self);

VALUE
rbc_pstring_comparator(VALUE self, VALUE other);

VALUE
rbc_pstring_append(VALUE self, VALUE rother);

#endif
