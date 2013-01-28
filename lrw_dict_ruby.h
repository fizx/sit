#ifndef LRW_DICT_RUBY_H_INCLUDED
#define LRW_DICT_RUBY_H_INCLUDED

#include "ruby.h"

VALUE
rbc_lrw_dict_new(VALUE class, VALUE size);

VALUE
rbc_lrw_dict_get(VALUE self, VALUE key);

VALUE
rbc_lrw_dict_put(VALUE self, VALUE key, VALUE value);

VALUE
rbc_lrw_dict_capacity(VALUE self);

VALUE
rbc_lrw_dict_size(VALUE self);

VALUE
rbc_lrw_dict_each(VALUE self);

#endif
