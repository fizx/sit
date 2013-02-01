#include "query_parser_ruby.h"
#include "query_parser.h"
#include "callback_ruby.h"
#include "sit_callback.h"
#include "pstring_ruby.h"

VALUE 
rbc_query_parser_new(VALUE class) {
  VALUE rcb = rbc_callback_new(
    rb_eval_string("::Sit::Callback"), 
    rb_eval_string("::Sit::Query"),
    rb_eval_string("proc{|q| @queries << q }")
  );
  rb_iv_set(rcb, "@queries", rb_ary_new());
	sit_callback *cb;
	Data_Get_Struct(rcb, sit_callback, cb);
  query_parser *qp = query_parser_new(cb);
	VALUE tdata = Data_Wrap_Struct(class, NULL, NULL, qp);
  rb_iv_set(tdata, "@callback", rcb);
  return tdata;
}

VALUE 
rbc_query_parser_consume(VALUE self, VALUE rstr) {
	query_parser *qp;
	Data_Get_Struct(self, query_parser, qp);
  query_parser_consume(qp, r2pstring(rstr));
  return Qnil;
}

VALUE 
rbc_query_parser_queries(VALUE self) {
  VALUE rcb = rb_iv_get(self, "@callback");
  return rb_iv_get(rcb, "@queries");
}
