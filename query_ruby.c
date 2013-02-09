#include "query_ruby.h"
#include "sit_query.h"
#include "util_ruby.h"
#include "ruby.h"
#include <stdlib.h>

VALUE
rbc_query_new(VALUE class, VALUE rconjunctions, VALUE rcallback) {
	int count = NUM2INT(rb_funcall(rconjunctions, rb_intern("size"), 0));
	conjunction_t **conjunctions = malloc(sizeof(conjunction_t) * count);
  VALUE con = rb_eval_string("::Sit::Conjunction");
	for (int i = 0; i < count; i++) {
		VALUE rcon = rb_ary_entry(rconjunctions, i);
		if(rb_obj_is_kind_of(rcon, con) != Qtrue) {
      rb_raise(rb_eTypeError, "Query constructor expects an array of conjunctions");
      return Qnil;
		}
		Data_Get_Struct(rcon, conjunction_t, conjunctions[i]);
	}
	sit_callback *cb;
	Data_Get_Struct(rcallback, sit_callback, cb);
	sit_query *query = sit_query_new(conjunctions, count, cb);
	
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, query);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE 
rbc_query_to_s(VALUE self) {
	sit_query *query;
	Data_Get_Struct(self, sit_query, query);
	VALUE buf = rb_ary_new();
	rb_ary_push(buf, rb_str_new2("<"));
	for(int i = 0; i < query->count; i++) {
	  if(i > 0) {
	    rb_ary_push(buf, rb_str_new2(" OR "));
    }
		conjunction_t *con = query->conjunctions[i];
  	VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::Conjunction"), markall, NULL, con);
  	rb_obj_call_init(tdata, 0, NULL);
  	rb_ary_push(buf, rb_funcall(tdata, rb_intern("to_s"), 0));
	}
	rb_ary_push(buf, rb_str_new2(" cb:"));
	rb_ary_push(buf, query->callback ? INT2NUM(query->callback->id) : Qnil);
	rb_ary_push(buf, rb_str_new2(">"));
	return rb_funcall(buf, rb_intern("join"), 0);
}

VALUE 
rbc_query_equals(VALUE self, VALUE other) {
	VALUE a = rb_funcall(self, rb_intern("to_s"), 0);
	VALUE b = rb_funcall(other, rb_intern("to_s"), 0);
	return rb_equal(a, b);
}

VALUE
rbc_conjunction_new(VALUE class, VALUE rterms) {
  int term_count = NUM2INT(rb_funcall(rterms, rb_intern("size"), 0));
	sit_term **terms = malloc(sizeof(sit_term) * term_count);
  VALUE term = rb_eval_string("::Sit::Term");
	for (int i = 0; i < term_count; i++) {
    VALUE rterm = rb_ary_entry(rterms, i);
		if(rb_obj_is_kind_of(rterm, term) != Qtrue) {
      rb_raise(rb_eTypeError, "Conjunction constructor expects an array of terms");
      return Qnil;
		}
		Data_Get_Struct(rterm, sit_term, terms[i]);
	}
	conjunction_t *con = conjunction_new(terms, term_count);
	
	VALUE tdata = Data_Wrap_Struct(class, NULL, free, con);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
	
}

VALUE 
rbc_conjunction_to_s(VALUE self) {
	conjunction_t *con;
	Data_Get_Struct(self, conjunction_t, con);
	VALUE buf = rb_ary_new();
	rb_ary_push(buf, rb_str_new2("("));
	for(int i = 0; i < con->count; i++) {
	  if(i > 0) {
	    rb_ary_push(buf, rb_str_new2(" AND "));
    }
		sit_term *term = &con->terms[i];
		if(term->negated) {
		  rb_ary_push(buf, rb_str_new2("NOT "));
		}
		char *str;
		asprintf(&str, "%.*s ~ %.*s", 
			term->field->len, term->field->val, 
			term->text->len, term->text->val);
		VALUE rstr = rb_str_new2(str);
		free(str);
		rb_ary_push(buf, rstr);
	}
	rb_ary_push(buf, rb_str_new2(")"));
	
	return rb_funcall(buf, rb_intern("join"), 0);
}

VALUE 
rbc_conjunction_equals(VALUE self, VALUE other) {
  VALUE a = rb_funcall(self, rb_intern("to_s"), 0);
	VALUE b = rb_funcall(other, rb_intern("to_s"), 0);
	return rb_equal(a, b);
}
