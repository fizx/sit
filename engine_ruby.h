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

#endif
