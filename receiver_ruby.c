#include "sit_ruby.h"

void 
_ruby_term_found(struct sit_receiver *receiver, pstring *pstr, int field_offset) {  
  VALUE rreceiver = vunwrap(receiver->data);
  VALUE rstr = p2rstring(pstr);
  VALUE rfield_offset = INT2NUM(field_offset);
  rb_funcall(rreceiver, rb_intern("term_found"), 2, rstr, rfield_offset);
}

void 
_ruby_document_found(struct sit_receiver *receiver, long off, int len) {
  VALUE rreceiver = vunwrap(receiver->data);
  VALUE roff = LONG2NUM(off);
  VALUE rlen = INT2NUM(len);
  rb_funcall(rreceiver, rb_intern("document_found"), 2, roff, rlen);
}

void 
_ruby_field_found(struct sit_receiver *receiver, pstring *name) {
  VALUE rreceiver = vunwrap(receiver->data);
  VALUE rstr = p2rstring(name);
  rb_funcall(rreceiver, rb_intern("field_found"), 1, rstr);
}

void 
_ruby_error_found(struct sit_receiver *receiver, pstring *message) {
  VALUE rreceiver = vunwrap(receiver->data);
  VALUE rstr = p2rstring(message);
  rb_funcall(rreceiver, rb_intern("error_found"), 1, rstr);
}

void 
_ruby_int_found(struct sit_receiver *receiver, int val) {
  VALUE rreceiver = vunwrap(receiver->data);
  VALUE rval = INT2NUM(val);
  rb_funcall(rreceiver, rb_intern("int_found"), 1, rval);
}

VALUE 
rbc_receiver_new(VALUE class) {
  sit_receiver *receiver = sit_receiver_new();
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, receiver);
  SET_ONCE(receiver->data, vwrap(tdata));
  receiver->term_found = _ruby_term_found;
  receiver->document_found = _ruby_document_found;
  receiver->field_found = _ruby_field_found;
  receiver->error_found = _ruby_error_found;
  receiver->int_found = _ruby_int_found;
	return tdata;
}