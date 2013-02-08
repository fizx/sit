#include "sit_input_ruby.h"
#include "sit_ruby.h"
#include "sit.h"

VALUE 
rbc_input_consume(VALUE self, VALUE rstr) {
	sit_input *input;
	Data_Get_Struct(self, sit_input, input);
	pstring *pstr = r2pstring(rstr);
	sit_input_consume(input, pstr);
	pstring_free(pstr);
	return Qnil;
}

VALUE
rbc_input_terms(VALUE self) {
	sit_input *input;
	Data_Get_Struct(self, sit_input, input);
	VALUE ary = rb_ary_new();
	for (int i = 0; i < input->term_count; i++) {
		sit_term *term = &input->terms[i];
		pstring *pstr = sit_term_to_s(term);
		rb_ary_push(ary, p2rstring(pstr));
		pstring_free(pstr);
	}
	return ary;
}
