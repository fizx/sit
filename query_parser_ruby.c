#include "query_parser_ruby.h"
#include "query_parser.h"
#include "callback_ruby.h"
#include "sit_callback.h"
#include "pstring_ruby.h"
#include "ruby.h"
#include "util_ruby.h"
#include <assert.h>

#define rbpush(cstr)    rb_ary_push(buf, rb_str_new2(cstr));

VALUE 
rbc_query_parser_new(VALUE class) {
  VALUE rcb = rbc_callback_new(
    rb_eval_string("::Sit::Callback"), 
    rb_eval_string("::Sit::Query"),
    rb_eval_string("proc{|q| $queries ||=[]; $queries << q }")
  );
	sit_callback *cb;
	Data_Get_Struct(rcb, sit_callback, cb);
  query_parser *qp = query_parser_new(cb);
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, qp);
  rb_iv_set(tdata, "@callback", rcb);
  return tdata;
}

VALUE 
rbc_query_parser_consume(VALUE self, VALUE rstr) {
	query_parser *qp;
	Data_Get_Struct(self, query_parser, qp);
  int status = query_parser_consume(qp, r2pstring(rstr));
  VALUE str = rb_str_new2("unknown");
  switch(status) {
    case 0: str = rb_str_new2("success"); break;
    case 1: str = rb_str_new2("invalid"); break;
    case 2: str = rb_str_new2("oom");     break;
    case 4: str = rb_str_new2("more");    break;
  }
  return rb_funcall(str, rb_intern("to_sym"), 0);
}

VALUE 
rbc_query_parser_queries(VALUE self) {
  VALUE rcb = rb_iv_get(self, "@callback");
  return rb_iv_get(rcb, "@queries");
}

VALUE
rbc_query_parser_last_error(VALUE self) {
	query_parser *qp;
	Data_Get_Struct(self, query_parser, qp);
  return p2rstring(qp->error);
}

void
_ast_to_s_recurse(ast_node_t* node, VALUE buf, int n) {
  if(node == NULL) return;
  for(int i =0; i < n; i++) {
    rb_ary_push(buf, rb_str_new2("\t"));
  }
  rb_ary_push(buf, p2rstring(query_node_to_s(node->data)));
  rb_ary_push(buf, rb_str_new2("\n"));
  _ast_to_s_recurse(node->child, buf, n + 1);
  _ast_to_s_recurse(node->next, buf, n);
}

VALUE 
rbc_query_parser_last_ast_to_s(VALUE self) {
	query_parser *qp;
	Data_Get_Struct(self, query_parser, qp);
  ast_node_t* node = qp->root;
  return p2rstring(query_node_ast_to_s(node));
}

VALUE
rbc_query_parser_last_query_to_s(VALUE self) {
  query_parser *qp;
	Data_Get_Struct(self, query_parser, qp);
	ast_node_t* node = qp->root;
  return p2rstring(query_node_query(node));
}