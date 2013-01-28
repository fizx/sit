#include "query_ruby.h"
#include "sit_query.h"
#include "util_ruby.h"
#include <stdlib.h>

void 
_query_free(sit_query *query) {
	free(query->terms);
	free(query);
}

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
	
	VALUE values = rb_ary_new();
	rb_funcall(rterms, rb_intern("sort!"), 0);
	rb_ary_push(values, rterms);
	rb_ary_push(values, rcallback);
	query->data = (void *) vwrap(values);

	VALUE tdata = Data_Wrap_Struct(class, NULL, _query_free, query);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE 
rbc_query_to_s(VALUE self) {
	sit_query *query;
	Data_Get_Struct(self, sit_query, query);
	VALUE ary = vunwrap(query->data);
	VALUE rstr = rb_funcall(ary, rb_intern("to_s"), 0);
	return rstr;
}
