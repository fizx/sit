#include "ring_buffer_ruby.h"
#include "pstring_ruby.h"

VALUE
rbc_ring_buffer_new(VALUE class, VALUE rsize) {
	long size = NUM2LONG(rsize);
	ring_buffer *rb = ring_buffer_new(size);
	VALUE tdata = Data_Wrap_Struct(class, NULL, ring_buffer_free, rb);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE 
rbc_ring_buffer_append(VALUE self, VALUE rstr) {
 	ring_buffer *rb;
	Data_Get_Struct(self, ring_buffer, rb);
	pstring *pstr = r2pstring(rstr);
	ring_buffer_append(rb, pstr);
	pstring_free(pstr);
	return Qnil;
}

VALUE 
rbc_ring_buffer_get(VALUE self, VALUE roff, VALUE rlen) {
	long off = NUM2LONG(roff);
	int len = NUM2INT(rlen);
 	ring_buffer *rb;
	Data_Get_Struct(self, ring_buffer, rb);
	pstring *pstr = ring_buffer_get(rb, off, len);
	VALUE rstr = p2rstring(pstr);
	pstring_free(pstr);
	return rstr;
}