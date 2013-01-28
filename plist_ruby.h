#ifndef PLIST_RUBY_H_INCLUDED
#define PLIST_RUBY_H_INCLUDED

#include "ruby.h"
#include "plist.h"

VALUE
rbc_plist_pool_new(VALUE class, VALUE rsize);

VALUE
rbc_plist_pool_capacity(VALUE self);

VALUE
rbc_plist_new(VALUE class, VALUE rpool);

VALUE
rbc_plist_size(VALUE self);

VALUE 
rbc_plist_entry_new(VALUE class, VALUE doc, VALUE pos);

VALUE
rbc_plist_append(VALUE self, VALUE entry);

VALUE
rbc_plist_each(VALUE self);

VALUE 
rbc_plist_entry_to_s(VALUE self);

VALUE 
rbc_plist_entry_equals(VALUE self, VALUE other);

#endif

