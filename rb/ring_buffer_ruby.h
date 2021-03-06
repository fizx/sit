#ifndef RING_BUFFER_RUBY_H_INCLUDED
#define RING_BUFFER_RUBY_H_INCLUDED

#include "ring_buffer.h"
#include "ruby.h"

VALUE
rbc_ring_buffer_new(VALUE class, VALUE size);

VALUE 
rbc_ring_buffer_append(VALUE self, VALUE rstr);

VALUE 
rbc_ring_buffer_get(VALUE self, VALUE off, VALUE len);

VALUE
rbc_int_ring_buffer_new(VALUE class, VALUE size);

VALUE 
rbc_int_ring_buffer_append(VALUE self, VALUE rint);

VALUE 
rbc_int_ring_buffer_get(VALUE self, VALUE off);

VALUE
rbc_int_ring_buffer_cursor_new(VALUE class, VALUE buffer, VALUE operator, VALUE predicate);

VALUE
rbc_int_ring_buffer_cursor_get(VALUE self);

VALUE
rbc_int_ring_buffer_cursor_pos(VALUE self);

VALUE
rbc_int_ring_buffer_cursor_prev(VALUE self);

#endif



