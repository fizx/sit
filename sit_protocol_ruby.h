#ifndef SIT_PROTOCOL_RUBY_H_INCLUDED
#define SIT_PROTOCOL_RUBY_H_INCLUDED

#include "ruby.h"

VALUE 
rbc_protocol_consume(VALUE self, VALUE rstr);

VALUE
rbc_protocol_new(VALUE class, VALUE rinput);

VALUE
rbc_protocol_end_stream(VALUE self);

#endif