#include "sit_ruby.h"

VALUE
rbc_engine_new(VALUE class, VALUE rparser, VALUE rdata_dir, VALUE rsize, VALUE rdedupe, VALUE rauth) {
	Parser *parser;
	Data_Get_Struct(rparser, Parser, parser);
  pstring *data_dir = r2pstring(rdata_dir);
  pstring *auth = r2pstring(rauth);
	long size = NUM2LONG(rsize);
	Engine *engine = engine_new(parser, data_dir, size, rdedupe == Qtrue, auth);
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, engine);
	rb_obj_call_init(tdata, 0, NULL);
	engine->parser = parser;
	SET_ONCE(engine->data, (void *) vwrap(tdata));
	return tdata;
}

VALUE
rbc_engine_last_document(VALUE self) {
	Engine *engine;
	Data_Get_Struct(self, Engine, engine);
  pstring *pstr = engine_last_document(engine);
  VALUE rstr = p2rstring(pstr);
  pstring_free(pstr);
  return rstr;
}

VALUE 
rbc_engine_search(VALUE self, VALUE rquery) {
	Engine *engine;
	Data_Get_Struct(self, Engine, engine);
  Query *query;
	Data_Get_Struct(rquery, Query, query);
  ResultIterator *iter = engine_search(engine, query);
  assert(iter);
	VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::ResultIterator"), markall, NULL, iter);
  return tdata;
}

VALUE
rbc_engine_min_document_id(VALUE self) {
  Engine *engine;
	Data_Get_Struct(self, Engine, engine);
  long id = engine_min_document_id(engine);
  return LONG2NUM(id);
}

VALUE
rbc_engine_get(VALUE self, VALUE key) {
  long n = NUM2LONG(key);
  Engine *engine;
	Data_Get_Struct(self, Engine, engine);
  pstring *str = engine_get_document(engine, n);
  VALUE rstr = p2rstring(str);
  pstring_free(str);
  return rstr;
}

VALUE 
rbc_engine_replay_journal(VALUE self) {
	Engine *engine;
	Data_Get_Struct(self, Engine, engine);
  engine_replay_journal(engine);
  return Qnil;
}

VALUE 
rbc_engine_append_journal(VALUE self, VALUE rstr) {
	Engine *engine;
	Data_Get_Struct(self, Engine, engine);
  pstring *str = r2pstring(rstr);
  engine_append_journal(engine, str);
  pstring_free(str);
  return Qnil;
}

VALUE 
rbc_engine_fsync_journal(VALUE self)  {
	Engine *engine;
	Data_Get_Struct(self, Engine, engine);
  engine_fsync_journal(engine);
  return Qnil;
}

VALUE 
rbc_engine_reopen_journal(VALUE self) {
	Engine *engine;
	Data_Get_Struct(self, Engine, engine);
  engine_reopen_journal(engine);
  return Qnil;
}


VALUE
rbc_result_iterator_prev(VALUE self) {
  ResultIterator *iter;
  Data_Get_Struct(self, ResultIterator, iter);
  return result_iterator_prev(iter) ? Qtrue : Qfalse;
}

VALUE
rbc_result_iterator_document(VALUE self) {
  ResultIterator *iter;
  Data_Get_Struct(self, ResultIterator, iter);
  pstring *pstr = result_iterator_document(iter);
  VALUE rstr = p2rstring(pstr);
  pstring_free(pstr);
  return rstr;
}

VALUE
rbc_result_iterator_document_id(VALUE self) {
  ResultIterator *iter;
  Data_Get_Struct(self, ResultIterator, iter);
  long id = result_iterator_document_id(iter);
  return LONG2NUM(id);
}

VALUE
rbc_engine_last_document_id(VALUE self) {
  Engine *engine;
	Data_Get_Struct(self, Engine, engine);
  long id = engine_last_document_id(engine);
  return LONG2NUM(id);
}

VALUE
rbc_result_iterator_call(VALUE self) {
  ResultIterator *iter;
  Data_Get_Struct(self, ResultIterator, iter);
  result_iterator_do_callback(iter);
  return Qnil;
}

VALUE
rbc_engine_get_document(VALUE self, VALUE rid) {
  Engine *engine;
	Data_Get_Struct(self, Engine, engine);
  long id = NUM2LONG(rid);
  pstring *pstr = engine_get_document(engine, id);
  VALUE rstr = p2rstring(pstr);
  pstring_free(pstr);
  return rstr;
}

VALUE
rbc_engine_parser(VALUE self) {	
	Engine *engine;
	Data_Get_Struct(self, Engine, engine);
	Parser *parser = engine->parser;
	return vunwrap(parser->data);
}

VALUE 
rbc_engine_get_int(VALUE self, VALUE rid, VALUE rfield) {
	Engine *engine;
	Data_Get_Struct(self, Engine, engine);
  pstring *pstr = r2pstring(rfield);
  long id = NUM2LONG(rid);
  int *ptr = engine_get_int(engine, id, pstr);
  if(ptr == NULL) {
    return Qnil;
  } else {
    return INT2NUM(*ptr);
  }
}

VALUE 
rbc_engine_set_int(VALUE self, VALUE rid, VALUE rfield, VALUE rvalue) {
  Engine *engine;
	Data_Get_Struct(self, Engine, engine);
  pstring *pstr = r2pstring(rfield);
  long id = NUM2LONG(rid);
  int value = NUM2INT(rvalue);
  engine_set_int(engine, id, pstr, value);
  return Qnil;
}

VALUE 
rbc_engine_incr(VALUE self, VALUE rid, VALUE rfield, VALUE rvalue) {
  Engine *engine;
	Data_Get_Struct(self, Engine, engine);
  pstring *pstr = r2pstring(rfield);
  long id = NUM2LONG(rid);
  int value = NUM2INT(rvalue);
  engine_incr(engine, id, pstr, value);
  return Qnil;
}


void _queries_handler(Callback *cb, void *vquery) {
	Query *query = vquery;
	VALUE ary = vunwrap(cb->user_data);
	VALUE class = rb_eval_string("::Sit::Query");
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, query);
	rb_obj_call_init(tdata, 0, NULL);
	rb_ary_push(ary, tdata);
}

VALUE 
rbc_engine_queries(VALUE self){
	Engine *engine;
	Data_Get_Struct(self, Engine, engine);
	Callback cb;
	VALUE ary = rb_ary_new();
	cb.user_data = (void *) vwrap(ary);
	cb.handler = _queries_handler;
	engine_each_query(engine, &cb);
	free(cb.user_data);
	return ary;
}

VALUE 
rbc_engine_unregister(VALUE self, VALUE id) {
	Engine *engine;
	Data_Get_Struct(self, Engine, engine);
	engine_unregister(engine, NUM2LONG(id));
	return Qnil;
}

VALUE 
rbc_engine_register(VALUE self, VALUE rquery) {
	Query *query;
	Data_Get_Struct(rquery, Query, query);
	
	Engine *engine;
	Data_Get_Struct(self, Engine, engine);
	
	int cb = engine_register(engine, query);
	
	return INT2NUM(cb);
}
