#include "engine_ruby.h"
#include "sit_engine.h"
#include "util_ruby.h"
#include "pstring_ruby.h"

VALUE
rbc_engine_new(VALUE class, VALUE rparser, VALUE rsize) {
	sit_parser *parser;
	Data_Get_Struct(rparser, sit_parser, parser);
	long size = NUM2LONG(rsize);
	sit_engine *engine = sit_engine_new(parser, size);
	VALUE tdata = Data_Wrap_Struct(class, NULL, NULL, engine);
	rb_obj_call_init(tdata, 0, NULL);
	rb_funcall(rparser, rb_intern("engine="), 1, tdata);
	engine->data = vwrap(tdata);
	return tdata;
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

void _queries_handler(void *vquery, void *data) {
	sit_query *query = vquery;
	VALUE ary = vunwrap(data);
	VALUE class = rb_eval_string("::Sit::Query");
	VALUE tdata = Data_Wrap_Struct(class, NULL, free, query);
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
rbc_engine_register(VALUE self, VALUE rquery) {
	sit_query *query;
	Data_Get_Struct(rquery, sit_query, query);
	
	sit_engine *engine;
	Data_Get_Struct(self, sit_engine, engine);
	
	int cb = sit_engine_register(engine, query);
	
	return INT2NUM(cb);
}
