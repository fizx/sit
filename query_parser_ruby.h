#ifndef PSTRING_RUBY_H_INCLUDED
#define PSTRING_RUBY_H_INCLUDED

VALUE 
rbc_query_parser_new(VALUE class);

VALUE 
rbc_query_parser_consume(VALUE self, VALUE rstr);

VALUE 
rbc_query_parser_queries(VALUE self);

#endif
