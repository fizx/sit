/**
 * Listing of all of the ruby classes to load.  The class definitions themselves
 * are either in Ruby inlined here (if they're really simple), or in <class>_ruby.c
 */

#include "ruby.h"
#include "pstring_ruby.h"
#include "ring_buffer_ruby.h"
#include "term_ruby.h"
#include "callback_ruby.h"
#include "query_ruby.h"

VALUE rbc_pstring;
VALUE rbc_ring_buffer;
VALUE rbc_term;
VALUE rbc_callback;
VALUE rbc_query;

void 
Init_sit() {
	VALUE m_sit = rb_define_module("Sit");
	
	// PString
	rbc_pstring = rb_define_class_under(m_sit, "PString", rb_cObject);
	rb_define_singleton_method(rbc_pstring, "new", rbc_pstring_new, 1);
	rb_define_method(rbc_pstring, "to_s", rbc_pstring_to_s, 0);
	
	// RingBuffer
	rbc_ring_buffer = rb_define_class_under(m_sit, "RingBuffer", rb_cObject);
	rb_define_singleton_method(rbc_ring_buffer, "new", rbc_ring_buffer_new, 1);
	rb_define_method(rbc_ring_buffer, "append", rbc_ring_buffer_append, 1);
	rb_define_method(rbc_ring_buffer, "get", rbc_ring_buffer_get, 2);
	
	// Term
	rbc_term = rb_define_class_under(m_sit, "Term", rb_cObject);
	rb_define_singleton_method(rbc_term, "new", rbc_term_new, 3);
	rb_define_method(rbc_term, "to_s", rbc_term_to_s, 0);

	// Callback
	rbc_callback = rb_define_class_under(m_sit, "Callback", rb_cObject);
	rb_define_singleton_method(rbc_callback, "new", rbc_callback_new, 2);
	rb_define_method(rbc_callback, "call", rbc_callback_call, 1);
	rb_define_method(rbc_callback, "to_s", rbc_callback_to_s, 0);

	// Query
	rbc_query = rb_define_class_under(m_sit, "Query", rb_cObject);
	rb_define_singleton_method(rbc_query, "new", rbc_query_new, 2);
	rb_define_method(rbc_query, "to_s", rbc_query_to_s, 0);

}