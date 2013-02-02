#include "query_parser_ruby.h"
#include "query_parser.h"
#include "callback_ruby.h"
#include "sit_callback.h"
#include "pstring_ruby.h"
#include "ruby.h"
#include <assert.h>

#define rbpush(cstr)    rb_ary_push(buf, rb_str_new2(cstr));

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

VALUE 
node_to_s(query_node *node) {
  VALUE buf;
  switch(node->type) {
    case NUM: 
      buf = INT2NUM(node->num);
      return StringValue(buf);
    case EXPR:
      buf = rb_ary_new();
      if (abs(node->logic) == 2) {
        rbpush(" OR ");
      } else if (abs(node->logic) == 1) {
        rbpush(" AND ");
      }
      if (node->logic < 0) {
        rbpush("NOT ");
      }
      rbpush("(");
      node = node->children;
      do {
        rb_ary_push(buf, node_to_s(node));
      } while ((node = node->next));
      rbpush(")");
      return rb_funcall(buf, rb_intern("join"), 0);
    case CLAUSE:
      buf = rb_ary_new();
      if (abs(node->logic) == 2) {
        rbpush(" OR ");
      } else if (abs(node->logic) == 1) {
        rbpush(" AND ");
      }
      if (node->logic < 0) {
        rbpush("NOT ");
      }
      rb_ary_push(buf, node_to_s(node->children));
      rbpush(" ");
      switch(node->cmp) {
        case _NA    : rbpush("WTF"); break;
        case _EQ    : rbpush("=="); break;
        case _GT    : rbpush(">"); break;
        case _LT    : rbpush("<"); break;
        case _GTE   : rbpush(">="); break;
        case _LTE   : rbpush("<="); break;
        case _TILDE : rbpush("~"); break;
        case _NEQ   : rbpush("!="); break;
      }
      rbpush(" ");
      rb_ary_push(buf, node_to_s(node->children->next));
      return rb_funcall(buf, rb_intern("join"), 0);
    case STR:
      return p2rstring(node->val);
    case MODSTR:
      return rb_str_new2("mod");
    case BOOLOP:
    case UNKNOWN:
    case CMP:
    default:
      assert(0);
  }
}

VALUE 
rbc_query_parser_last_ast_to_s(VALUE self) {
	query_parser *qp;
	Data_Get_Struct(self, query_parser, qp);
	query_node *ast = qp->ast;
	if(ast == NULL) {
    return Qnil;
  } else {
    return node_to_s(ast);
  }
}