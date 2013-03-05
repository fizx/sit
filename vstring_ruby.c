#include "sit_ruby.h"

VALUE
rbc_vstring_new(VALUE class) {
  vstring *ptr = vstring_new();	
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, ptr);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE
rbc_vstring_append(VALUE self, VALUE rstr) {
  vstring *vstr;
  Data_Get_Struct(self, vstring, vstr);
  pstring *pstr = r2pstring(rstr);
  vstring_append(vstr, pstr);
  pstring_free(pstr);
  return Qnil;
}

VALUE 
rbc_vstring_get(VALUE self, VALUE roff, VALUE rlen) {
  vstring *vstr;
  Data_Get_Struct(self, vstring, vstr);
  long off = NUM2LONG(roff);
  int len = NUM2INT(rlen);
  pstring pstr = { NULL, len };
  vstring_get(&pstr, vstr, off);
  VALUE out = p2rstring(&pstr);
  return out;
}

VALUE
rbc_vstring_shift(VALUE self, VALUE count) {
  vstring *vstr;
  Data_Get_Struct(self, vstring, vstr);
  long off = NUM2LONG(count);
  vstring_shift(vstr, off);
  return Qnil;
}

VALUE
rbc_vstring_to_s(VALUE self) {
  VALUE size = rbc_vstring_size(self);
  return rbc_vstring_get(self, INT2NUM(0), size);
}

VALUE
rbc_vstring_nodes(VALUE self) {
  vstring *vstr;
  Data_Get_Struct(self, vstring, vstr);
  VALUE ary = rb_ary_new();
  VStringNode *node = vstr->node;
  while(node) {
    rb_ary_push(ary, p2rstring(&node->pstr));
    node = node->prev;
  }
  return ary;
}

VALUE
rbc_vstring_size(VALUE self) {
  vstring *vstr;
  Data_Get_Struct(self, vstring, vstr);
  return LONG2NUM(vstring_size(vstr));
}