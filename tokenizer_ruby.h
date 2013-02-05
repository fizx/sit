#ifndef TOKENIZER_RUBY_H_INCLUDED
#define TOKENIZER_RUBY_H_INCLUDED

#include "ruby.h"

VALUE 
rbc_tokenizer_new(VALUE class);

VALUE 
rbc_tokenizer_consume(VALUE self, VALUE rstr);

VALUE 
rbc_tokenizer_term_found(VALUE self, VALUE roff, VALUE rlen, VALUE rfield_offset);

VALUE
rbc_tokenizer_end_stream(VALUE self);

#endif
