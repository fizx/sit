#include "query_ruby.h"
#include "sit_query.h"
#include "util_ruby.h"
#include "ruby.h"
#include <stdlib.h>

VALUE
rbc_query_new(VALUE class, VALUE rterms, VALUE rcallback) {
	int term_count = NUM2INT(rb_funcall(rterms, rb_intern("size"), 0));
	sit_term **terms = malloc(sizeof(sit_term) * term_count);
	for (int i = 0; i < term_count; i++) {
		VALUE rterm = rb_ary_entry(rterms, i);
		Data_Get_Struct(rterm, sit_term, terms[i]);
	}
	sit_callback *cb;
	Data_Get_Struct(rcallback, sit_callback, cb);
	sit_query *query = sit_query_new(terms, term_count, cb);
	
	VALUE tdata = Data_Wrap_Struct(class, NULL, free, query);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE 
rbc_query_to_s(VALUE self) {
	sit_query *query;
	Data_Get_Struct(self, sit_query, query);
	VALUE buf = rb_ary_new();
	rb_ary_push(buf, rb_str_new2("<query"));
	for(int i = 0; i < query->term_count; i++) {
		sit_term *term = &query->terms[i];
		char *str;
		asprintf(&str, " [%.*s:%.*s %d]", 
			term->field->len, term->field->val, 
			term->text->len, term->text->val, 
			term->offset);
		VALUE rstr = rb_str_new2(str);
		free(str);
		rb_ary_push(buf, rstr);
	}
	rb_ary_push(buf, rb_str_new2("[cb:"));
	rb_ary_push(buf, INT2NUM(query->callback->id));
	rb_ary_push(buf, rb_str_new2("]>"));
	
	return rb_funcall(buf, rb_intern("join"), 0);
}

VALUE 
rbc_query_equals(VALUE self, VALUE other) {
	VALUE a = rb_funcall(self, rb_intern("to_s"), 0);
	VALUE b = rb_funcall(other, rb_intern("to_s"), 0);
	return rb_equal(a, b);
}
