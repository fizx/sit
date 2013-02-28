#ifndef ENGINE_RUBY_H_INCLUDED
#define ENGINE_RUBY_H_INCLUDED

#include "ruby.h"

VALUE
rbc_engine_new(VALUE class, VALUE parser, VALUE size);

VALUE
rbc_engine_parser(VALUE self);

VALUE 
rbc_engine_consume(VALUE self, VALUE rstr);

VALUE
rbc_engine_terms(VALUE self);

VALUE 
rbc_engine_register(VALUE self, VALUE rquery);

VALUE 
rbc_engine_queries(VALUE self);

VALUE 
rbc_engine_unregister(VALUE self, VALUE id);

VALUE
rbc_engine_last_document(VALUE self);

VALUE
rbc_engine_last_document_id(VALUE self);

VALUE
rbc_engine_get_document(VALUE self, VALUE rid);

VALUE 
rbc_engine_search(VALUE self, VALUE query);

VALUE
rbc_result_iterator_prev(VALUE self);

VALUE
rbc_result_iterator_document(VALUE self);

VALUE
rbc_result_iterator_document_id(VALUE self);

VALUE
rbc_result_iterator_call(VALUE self);

VALUE 
rbc_engine_get_int(VALUE self, VALUE rid, VALUE rfield);

VALUE 
rbc_engine_set_int(VALUE self, VALUE rid, VALUE rfield, VALUE rvalue);

VALUE 
rbc_engine_incr(VALUE self, VALUE rid, VALUE rfield, VALUE rvalue);

VALUE 
rbc_engine_document_found(VALUE self, VALUE rstr);

VALUE 
rbc_engine_field_found(VALUE self, VALUE rstr);

VALUE 
rbc_engine_int_found(VALUE self, VALUE rval);

VALUE 
rbc_engine_term_found(VALUE self, VALUE rstr, VALUE rfield_offset);

#endif
