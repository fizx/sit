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
  doc_buf_doc_found(buf, r2pstring(rstr));
  return Qnil;
}

VALUE
rbc_doc_buf_ints(VALUE self) {
  DocBuf *buf;
	Data_Get_Struct(self, DocBuf, buf);
  VALUE hash = rb_hash_new();
  dictIterator * iterator = dictGetIterator(buf->ints);
	dictEntry *next;
	while((next = dictNext(iterator))) {
    pstring *pstr = dictGetKey(next);
    int val = dictGetSignedIntegerVal(next);
    rb_hash_aset(hash, p2rstring(pstr), INT2NUM(val));
	}
  dictReleaseIterator(iterator);
  return hash;  
}

VALUE
rbc_doc_buf_terms(VALUE self) {
  DocBuf *buf;
	Data_Get_Struct(self, DocBuf, buf);
  VALUE array = rb_ary_new();
  dictIterator * iterator = dictGetIterator(buf->term_index);
	dictEntry *next;
  VALUE class = rb_eval_string("::Sit::Term");
	while((next = dictNext(iterator))) {
    Term *term = dictGetKey(next);
    VALUE tdata = Data_Wrap_Struct(class, markall, NULL, term);
  	rb_obj_call_init(tdata, 0, NULL);  	
    rb_ary_push(array, tdata);
	}
  dictReleaseIterator(iterator);
  return array;  
}

VALUE
rbc_doc_buf_doc(VALUE self) {
  DocBuf *buf;
	Data_Get_Struct(self, DocBuf, buf);
  return p2rstring(buf->doc);
}
