#include "sit_ruby.h"

VALUE
rbc_term_new(VALUE class, VALUE rfield, VALUE rtext, VALUE roff, VALUE rnegated) {
	pstring *field = r2pstring(rfield);
	pstring *text  = r2pstring(rtext);
  bool negated = rnegated == Qtrue;
	int off = NUM2INT(roff);
	assert(field);
	assert(text);
	Term *term = term_new(field, text, off, negated);
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, term);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE 
rbc_term_new_numeric(VALUE class, VALUE rfield, VALUE op, VALUE val) {
  pstring *field = r2pstring(rfield);
	pstring *text  = r2pstring(op);
	int off = NUM2INT(val);
	assert(field);
	assert(text);
	Term *term = term_new(field, text, off, false);
  term->type = NUMERIC;
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, term);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE
rbc_term_to_s(VALUE self) {
	Term *term;
	Data_Get_Struct(self, Term, term);
	char *str;
	asprintf(&str, "[%.*s:%.*s %d]", 
		term->field.len, term->field.val, 
		term->text.len, term->text.val, 
		term->offset);
	VALUE rstr = rb_str_new2(str);
	free(str);
	return rstr;
}

VALUE
rbc_term_comparator(VALUE self, VALUE other) {
	Term *a;
	Term *b;
	Data_Get_Struct(self, Term, a);
	Data_Get_Struct(other, Term, b);
	return INT2NUM(termcmp(a, b));	
}
