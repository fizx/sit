#ifndef QUERY_PARSER_RUBY_H_INCLUDED
#define QUERY_PARSER_RUBY_H_INCLUDED

#include "ruby.h"

VALUE 
rbc_query_parser_new(VALUE class);

VALUE 
rbc_query_parser_consume(VALUE self, VALUE rstr);

VALUE 
rbc_query_parser_queries(VALUE self);

VALUE
rbc_query_parser_last_error(VALUE self);

VALUE 
rbc_query_parser_last_ast_to_s(VALUE self);

#endif
