#include "ruby.h"
#include "pstring.h"
#include "pstring_ruby.h"
#include <string.h>

pstring *
r2pstring(VALUE rstr) {
	rstr = StringValue(rstr);
	pstring *pstr = pstring_new(RSTRING_LEN(rstr));
	strncpy(pstr->val, RSTRING_PTR(rstr), pstr->len);
	return pstr;
}

VALUE
p2rstring(pstring *pstr) {
	if(pstr == NULL) {
		return Qnil;
	} else {
		return rb_str_new(pstr->val, pstr->len);
	}
}

VALUE
rbc_pstring_new(VALUE class, VALUE rstr) {
	pstring *ptr = r2pstring(rstr);	
	VALUE tdata = Data_Wrap_Struct(class, NULL, pstring_free, ptr);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE
rbc_pstring_to_s(VALUE self) {
	pstring *pstr;
	Data_Get_Struct(self, pstring, pstr);
	return p2rstring(pstr);
}

