#include "term_ruby.h"
#include "sit_term.h"
#include "pstring.h"
#include "pstring_ruby.h"
#include <assert.h>

VALUE
rbc_term_new(VALUE class, VALUE rfield, VALUE rtext, VALUE roff) {
	pstring *field = r2pstring(rfield);
	pstring *text  = r2pstring(rtext);
	int off = NUM2INT(roff);
	assert(field);
	assert(text);
	sit_term *term = sit_term_new(field, text, off);
	VALUE tdata = Data_Wrap_Struct(class, NULL, NULL, term);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE
rbc_term_to_s(VALUE self) {
	sit_term *term;
	Data_Get_Struct(self, sit_term, term);
	char *str;
	asprintf(&str, "[%.*s:%.*s %d]", 
		term->field->len, term->field->val, 
		term->text->len, term->text->val, 
		term->offset);
	VALUE rstr = rb_str_new2(str);
	free(str);
	return rstr;
}

VALUE
rbc_term_comparator(VALUE self, VALUE other) {
	sit_term *a;
	sit_term *b;
	Data_Get_Struct(self, sit_term, a);
	Data_Get_Struct(other, sit_term, b);
	return INT2NUM(sit_termcmp(a, b));	
}
