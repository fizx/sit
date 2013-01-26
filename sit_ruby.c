/**
 * Listing of all of the ruby classes to load.  The class definitions themselves
 * are either in Ruby inlined here (if they're really simple), or in <class>_ruby.c
 */

#include "ruby.h"
#include "pstring_ruby.h"

VALUE rbc_pstring;

void 
Init_sit() {
	VALUE m_sit = rb_define_module("Sit");
	
	// PString
	rbc_pstring = rb_define_class_under(m_sit, "PString", rb_cObject);
	rb_define_singleton_method(rbc_pstring, "new", rbc_pstring_new, 1);
	rb_define_method(rbc_pstring, "to_s", rbc_pstring_to_s, 0);
}