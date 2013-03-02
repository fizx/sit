#ifndef DOC_BUF_RUBY_H_INCLUDED
#define DOC_BUF_RUBY_H_INCLUDED

#include "ruby.h"

VALUE 
rbc_doc_buf_new(VALUE class);

VALUE
rbc_doc_buf_field_found(VALUE self, VALUE rstr);

VALUE
rbc_doc_buf_term_found(VALUE self, VALUE rstr, VALUE roffset);

VALUE
rbc_doc_buf_int_found(VALUE self, VALUE rval);

VALUE
rbc_doc_buf_doc_found(VALUE self, VALUE rstr);

VALUE
rbc_doc_buf_ints(VALUE self);

VALUE
rbc_doc_buf_terms(VALUE self);

VALUE
rbc_doc_buf_doc(VALUE self);


#endif