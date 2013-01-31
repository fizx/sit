#ifndef PARSER_RUBY_H_INCLUDED
#define PARSER_RUBY_H_INCLUDED

#include "ruby.h"

VALUE 
rbc_parser_new(VALUE class);

VALUE 
rbc_parser_initialize(VALUE class);

VALUE
rbc_parser_set_engine(VALUE self, VALUE rengine);

VALUE
rbc_parser_consume(VALUE self, VALUE rstr);

VALUE
rbc_parser_term_found(VALUE self, VALUE off, VALUE len, VALUE field_offset);

VALUE
rbc_parser_document_found(VALUE self, VALUE off, VALUE len);

VALUE 
rbc_parser_field_found(VALUE self, VALUE rstr);

VALUE 
rbc_parser_int_found(VALUE self, VALUE rint);

VALUE 
rbc_parser_engine(VALUE self);

#endif

