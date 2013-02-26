#include "util.h"
#include "engine_ruby.h"
#include "sit_engine.h"
#include "util_ruby.h"
#include "pstring_ruby.h"
#include <assert.h>

VALUE
rbc_engine_new(VALUE class, VALUE rparser, VALUE rsize) {
	sit_parser *parser;
	Data_Get_Struct(rparser, sit_parser, parser);
	long size = NUM2LONG(rsize);
	sit_engine *engine = sit_engine_new(parser, size);
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, engine);
	rb_obj_call_init(tdata, 0, NULL);
	rb_funcall(rparser, rb_intern("receiver="), 1, tdata);
	engine->parser = parser;
	SET_ONCE(engine->data, (void *) vwrap(tdata));
	return tdata;
}

VALUE
rbc_engine_last_document(VALUE self) {
	sit_engine *engine;
	Data_Get_Struct(self, sit_engine, engine);
  pstring *pstr = sit_engine_last_document(engine);
  VALUE rstr = p2rstring(pstr);
  pstring_free(pstr);
  return rstr;
}

VALUE 
rbc_engine_search(VALUE self, VALUE rquery) {
	sit_engine *engine;
	Data_Get_Struct(self, sit_engine, engine);
  sit_query *query;
	Data_Get_Struct(rquery, sit_query, query);
  sit_result_iterator *iter = sit_engine_search(engine, query);
  assert(iter);
	VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::ResultIterator"), markall, NULL, iter);
  return tdata;
}

VALUE
rbc_result_iterator_prev(VALUE self) {
  sit_result_iterator *iter;
  Data_Get_Struct(self, sit_result_iterator, iter);
  return sit_result_iterator_prev(iter) ? Qtrue : Qfalse;
}

VALUE
rbc_result_iterator_document(VALUE self) {
  sit_result_iterator *iter;
  Data_Get_Struct(self, sit_result_iterator, iter);
  pstring *pstr = sit_result_iterator_document(iter);
  VALUE rstr = p2rstring(pstr);
  pstring_free(pstr);
  return rstr;
}

VALUE
rbc_result_iterator_document_id(VALUE self) {
  sit_result_iterator *iter;
  Data_Get_Struct(self, sit_result_iterator, iter);
  long id = sit_result_iterator_document_id(iter);
  return LONG2NUM(id);
}

VALUE
rbc_engine_last_document_id(VALUE self) {
  sit_engine *engine;
	Data_Get_Struct(self, sit_engine, engine);
  long id = sit_engine_last_document_id(engine);
  return LONG2NUM(id);
}

VALUE
rbc_result_iterator_call(VALUE self) {
  sit_result_iterator *iter;
  Data_Get_Struct(self, sit_result_iterator, iter);
  sit_result_iterator_do_callback(iter);
  return Qnil;
}

VALUE
rbc_engine_get_document(VALUE self, VALUE rid) {
  sit_engine *engine;
	Data_Get_Struct(self, sit_engine, engine);
  long id = NUM2LONG(rid);
  pstring *pstr = sit_engine_get_document(engine, id);
  VALUE rstr = p2rstring(pstr);
  pstring_free(pstr);
  return rstr;
}

VALUE
rbc_engine_parser(VALUE self) {	
	sit_engine *engine;
	Data_Get_Struct(self, sit_engine, engine);
	sit_parser *parser = engine->parser;
	return vunwrap(parser->data);
}

VALUE 
rbc_engine_consume(VALUE self, VALUE rstr) {
	sit_engine *engine;
	Data_Get_Struct(self, sit_engine, engine);
	pstring *pstr = r2pstring(rstr);
	sit_engine_consume(engine, pstr);
	pstring_free(pstr);
	return Qnil;
}

VALUE
rbc_engine_terms(VALUE self) {
	sit_engine *engine;
	Data_Get_Struct(self, sit_engine, engine);
	VALUE ary = rb_ary_new();
	for (int i = 0; i < engine->term_count; i++) {
		sit_term *term = &engine->terms[i];
		char *str;
		asprintf(&str, "[%.*s:%.*s %d]", 
			term->field->len, term->field->val, 
			term->text->len, term->text->val, 
			term->offset);
		VALUE rstr = rb_str_new2(str);
		free(str);
		rb_ary_push(ary, rstr);
	}
	return ary;
}

VALUE 
rbc_engine_get_int(VALUE self, VALUE rid, VALUE rfield) {
	sit_engine *engine;
	Data_Get_Struct(self, sit_engine, engine);
  pstring *pstr = r2pstring(rfield);
  long id = NUM2LONG(rid);
  int *ptr = sit_engine_get_int(engine, id, pstr);
  if(ptr == NULL) {
    return Qnil;
  } else {
    return INT2NUM(*ptr);
  }
}

VALUE 
rbc_engine_set_int(VALUE self, VALUE rid, VALUE rfield, VALUE rvalue) {
  sit_engine *engine;
	Data_Get_Struct(self, sit_engine, engine);
  pstring *pstr = r2pstring(rfield);
  long id = NUM2LONG(rid);
  int value = NUM2INT(rvalue);
  sit_engine_set_int(engine, id, pstr, value);
  return Qnil;
}

VALUE 
rbc_engine_incr(VALUE self, VALUE rid, VALUE rfield, VALUE rvalue) {
  sit_engine *engine;
	Data_Get_Struct(self, sit_engine, engine);
  pstring *pstr = r2pstring(rfield);
  long id = NUM2LONG(rid);
  int value = NUM2INT(rvalue);
  sit_engine_incr(engine, id, pstr, value);
  return Qnil;
}


void _queries_handler(sit_callback *cb, void *vquery) {
	sit_query *query = vquery;
	VALUE ary = vunwrap(cb->user_data);
	VALUE class = rb_eval_string("::Sit::Query");
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, query);
	rb_obj_call_init(tdata, 0, NULL);
	rb_ary_push(ary, tdata);
}

VALUE 
rbc_engine_queries(VALUE self){
	sit_engine *engine;
	Data_Get_Struct(self, sit_engine, engine);
	sit_callback cb;
	VALUE ary = rb_ary_new();
	cb.user_data = (void *) vwrap(ary);
	cb.handler = _queries_handler;
	sit_engine_each_query(engine, &cb);
	free(cb.user_data);
	return ary;
}

VALUE 
rbc_engine_unregister(VALUE self, VALUE id) {
	sit_engine *engine;
	Data_Get_Struct(self, sit_engine, engine);
	sit_engine_unregister(engine, NUM2LONG(id));
	return Qnil;
}

VALUE 
rbc_engine_document_found(VALUE self, VALUE roff, VALUE rlen) {
  sit_engine *engine;
	Data_Get_Struct(self, sit_engine, engine);
  long off = NUM2LONG(roff);
  int len = NUM2INT(rlen);
  sit_engine_document_found((sit_receiver*)engine, off, len);
	return Qnil;
}

VALUE 
rbc_engine_field_found(VALUE self, VALUE rstr) {
  sit_engine *engine;
	Data_Get_Struct(self, sit_engine, engine);
  pstring *pstr = r2pstring(rstr);
  sit_engine_field_found((sit_receiver*)engine, pstr);
	return Qnil;
}

VALUE 
rbc_engine_int_found(VALUE self, VALUE rval) {
  sit_engine *engine;
	Data_Get_Struct(self, sit_engine, engine);
  int val = NUM2INT(rval);
  sit_engine_int_found((sit_receiver*)engine, val);
	return Qnil;
}

VALUE 
rbc_engine_term_found(VALUE self, VALUE rstr, VALUE rfield_offset) {
  sit_engine *engine;
	Data_Get_Struct(self, sit_engine, engine);
  pstring *str = r2pstring(rstr);
  int field_offset = NUM2INT(rfield_offset);
  sit_engine_term_found((sit_receiver*)engine, str, field_offset);
	return Qnil;
}

VALUE 
rbc_engine_register(VALUE self, VALUE rquery) {
	sit_query *query;
	Data_Get_Struct(rquery, sit_query, query);
	
	sit_engine *engine;
	Data_Get_Struct(self, sit_engine, engine);
	
	int cb = sit_engine_register(engine, query);
	
	return INT2NUM(cb);
}
