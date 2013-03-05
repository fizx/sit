#include "sit_ruby.h"

pstring *
r2pstring(VALUE rstr) {
	rstr = StringValue(rstr);
  return pstring_new2(RSTRING_PTR(rstr), RSTRING_LEN(rstr));
}

VALUE
p2rstring(pstring *pstr) {
	if(pstr == NULL || pstr->len < 0) {
		return Qnil;
	} else {
    return rb_str_new(pstr->val, pstr->len);
	}
}

VALUE
rbc_pstring_new(VALUE class, VALUE rstr) {
	pstring *ptr = r2pstring(rstr);	
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, ptr);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE
rbc_pstring_to_s(VALUE self) {
	pstring *pstr;
	Data_Get_Struct(self, pstring, pstr);
	return p2rstring(pstr);
}

VALUE
rbc_pstring_comparator(VALUE self, VALUE other) {
	pstring *a;
	pstring *b;
	Data_Get_Struct(self, pstring, a);
	Data_Get_Struct(other, pstring, b);
	return INT2NUM(pstrcmp(a, b));
}

VALUE
rbc_pstring_append(VALUE self, VALUE rother) {
  pstring *pstr;
	Data_Get_Struct(self, pstring, pstr);
  VALUE rs = StringValue(rother);
  pstring *other = r2pstring(rs);
  padd(pstr, other);
  free(other);
  return p2rstring(pstr);
}
