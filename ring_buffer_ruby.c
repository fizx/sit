#include "sit_ruby.h"

VALUE
rbc_ring_buffer_new(VALUE class, VALUE rsize) {
	long size = NUM2LONG(rsize);
	ring_buffer *rb = ring_buffer_new(size);
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, rb);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE 
rbc_ring_buffer_append(VALUE self, VALUE rstr) {
 	ring_buffer *rb;
	Data_Get_Struct(self, ring_buffer, rb);
	pstring *pstr = r2pstring(rstr);
	ring_buffer_append_pstring(rb, pstr);
	pstring_free(pstr);
	return Qnil;
}

VALUE 
rbc_ring_buffer_get(VALUE self, VALUE roff, VALUE rlen) {
	long off = NUM2LONG(roff);
	int len = NUM2INT(rlen);
 	ring_buffer *rb;
	Data_Get_Struct(self, ring_buffer, rb);
	pstring *pstr = ring_buffer_get_pstring(rb, off, len);
	VALUE rstr = p2rstring(pstr);
	pstring_free(pstr);
	return rstr;
}

VALUE
rbc_int_ring_buffer_new(VALUE class, VALUE rsize) {
  long size = NUM2LONG(rsize);
	ring_buffer *rb = ring_buffer_new(size * sizeof(int));
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, rb);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE 
rbc_int_ring_buffer_append(VALUE self, VALUE rint) {
  int val = NUM2INT(rint);
	ring_buffer *rb;
	Data_Get_Struct(self, ring_buffer, rb);
	ring_buffer_append(rb, &val, sizeof(int));
  return Qnil;
}

VALUE 
rbc_int_ring_buffer_get(VALUE self, VALUE roff) {
  int len = sizeof(int);
  long off = NUM2LONG(roff) * len;
 	ring_buffer *rb;
	Data_Get_Struct(self, ring_buffer, rb);
  int *val = (int*) ring_buffer_get(rb, off, len);
  if(val) {
	  return INT2NUM(*val); 
	} else {
    return Qnil;
	}
}

VALUE
rbc_int_ring_buffer_cursor_new(VALUE class, VALUE buffer, VALUE operator, VALUE rpredicate) {
  ring_buffer *rb;
	Data_Get_Struct(buffer, ring_buffer, rb);
  ring_buffer_cursor *rbc;
  if (operator == Qnil) {
    rbc = ring_buffer_cursor_new(rb, sizeof(int));
  } else {
    operator = StringValue(operator);
    char op = *RSTRING_PTR(operator);
    int predicate = NUM2INT(rpredicate);
    rbc = ring_buffer_predicate_int_cursor_new(rb, sizeof(int), op, predicate);
  }
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, rbc);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE
rbc_int_ring_buffer_cursor_get(VALUE self) {
 	ring_buffer_cursor *rbc;
	Data_Get_Struct(self, ring_buffer_cursor, rbc);
  int *val = (int*)rbc->as_cursor.data;
  if(val) {
	  return INT2NUM(*val); 
	} else {
    return Qnil;
	}
}

VALUE
rbc_int_ring_buffer_cursor_pos(VALUE self) {
  ring_buffer_cursor *rbc;
	Data_Get_Struct(self, ring_buffer_cursor, rbc);
  return LONG2NUM(rbc->pos);
}

VALUE
rbc_int_ring_buffer_cursor_prev(VALUE self) {
  ring_buffer_cursor *rbc;
	Data_Get_Struct(self, ring_buffer_cursor, rbc);
	sit_cursor *cursor = &rbc->as_cursor;
  return cursor->prev(cursor) ? Qtrue : Qfalse;
}
