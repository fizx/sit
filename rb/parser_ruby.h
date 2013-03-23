#ifndef PARSER_RUBY_H_INCLUDED
#define PARSER_RUBY_H_INCLUDED

#include "ruby.h"

VALUE 
rbc_parser_new(VALUE class);

VALUE 
rbc_parser_new_json(VALUE class);

VALUE 
rbc_parser_new_solr(VALUE class);

VALUE 
rbc_parser_new_syslog(VALUE class);

VALUE
rbc_parser_new_regex(VALUE class, VALUE spec);

VALUE 
rbc_parser_new_whitespace(VALUE class);

VALUE 
rbc_whitespace_parser_new(VALUE class);

VALUE
rbc_parser_set_receiver(VALUE self, VALUE rengine);

VALUE
rbc_parser_consume(VALUE self, VALUE rstr);

VALUE
rbc_parser_term_found(VALUE self, VALUE rstr, VALUE field_offset);

VALUE
rbc_parser_document_found(VALUE self, VALUE rstr);

VALUE 
rbc_parser_field_found(VALUE self, VALUE rstr);

VALUE 
rbc_parser_int_found(VALUE self, VALUE rint);

VALUE 
rbc_parser_end_stream(VALUE self);

VALUE
rbc_parser_on_document(VALUE self, VALUE proc);

#endif

