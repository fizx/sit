#ifndef TOKENIZER_RUBY_H_INCLUDED
#define TOKENIZER_RUBY_H_INCLUDED

#include "ruby.h"

VALUE 
rbc_tokenizer_new_whitespace(VALUE class, VALUE tokens);

VALUE
rbc_tokenizer_consume(VALUE self, VALUE rstr);

VALUE
rbc_tokenizer_new_regex(VALUE class, VALUE patten, VALUE tokens);

VALUE 
rbc_tokenizer_end_stream(VALUE self);

#endif
