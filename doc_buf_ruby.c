#include "sit_ruby.h"

VALUE 
rbc_doc_buf_new(VALUE class) {
  DocBuf *buf = doc_buf_new();
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, buf);
  return tdata;
}

VALUE
rbc_doc_buf_field_found(VALUE self, VALUE rstr) {
  DocBuf *buf;
	Data_Get_Struct(self, DocBuf, buf);
  doc_buf_field_found(buf, r2pstring(rstr));
  return Qnil;
}

VALUE
rbc_doc_buf_term_found(VALUE self, VALUE rstr, VALUE roffset) {
  DocBuf *buf;
	Data_Get_Struct(self, DocBuf, buf);
  doc_buf_term_found(buf, r2pstring(rstr), NUM2INT(roffset));
  return Qnil;
}

VALUE
rbc_doc_buf_int_found(VALUE self, VALUE rval) {
  DocBuf *buf;
	Data_Get_Struct(self, DocBuf, buf);
  doc_buf_int_found(buf, NUM2INT(rval));
  return Qnil;
}

VALUE
rbc_doc_buf_doc_found(VALUE self, VALUE rstr) {
  DocBuf *buf;
	Data_Get_Struct(self, DocBuf, buf);
  doc_buf_field_found(buf, r2pstring(rstr));
  return Qnil;
}
