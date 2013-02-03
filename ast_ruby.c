#include "ast_ruby.h"
#include "ast.h"
#include "util_ruby.h"

void *
wrapnil() {
  return vwrap(Qnil);
}

VALUE 
rbc_ast_new(VALUE class) {
  ast_t *ast = ast_new(wrapnil, free);
	VALUE tdata = Data_Wrap_Struct(class, NULL, NULL, ast);
	rb_obj_call_init(tdata, 0, NULL);
  return tdata;
}

VALUE 
rbc_ast_node_new(VALUE class, VALUE rast, VALUE rdata) {
  ast_t *ast;
  Data_Get_Struct(rast, ast_t, ast);
  ast_node_t *node = ast_node_new(ast);  
	VALUE tdata = Data_Wrap_Struct(class, NULL, NULL, node);
	rb_obj_call_init(tdata, 0, NULL);
  rbc_ast_node_set_data(tdata, rdata);
  return tdata;
}

VALUE
rbc_ast_node_get_data(VALUE self) {
  ast_node_t *node;
  Data_Get_Struct(self, ast_node_t, node);
  return vunwrap(node->data);
}

VALUE
rbc_ast_node_set_data(VALUE self, VALUE rdata) {
  ast_node_t *node;
  Data_Get_Struct(self, ast_node_t, node);
  ((value_holder *)node->data)->val = rdata;
  return rdata;
}

VALUE 
rbc_ast_node_next(VALUE self) {
  ast_node_t *node;
  Data_Get_Struct(self, ast_node_t, node);
  if(node->next == NULL) return Qnil;
  VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::ASTNode"), NULL, NULL, node->next);
	rb_obj_call_init(tdata, 0, NULL);
  return tdata;
}

VALUE 
rbc_ast_node_prev(VALUE self) {
  ast_node_t *node;
  Data_Get_Struct(self, ast_node_t, node);
  if(node->prev == NULL) return Qnil;
  VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::ASTNode"), NULL, NULL, node->prev);
	rb_obj_call_init(tdata, 0, NULL);
  return tdata;
}

VALUE 
rbc_ast_node_parent(VALUE self) {
  ast_node_t *node;
  Data_Get_Struct(self, ast_node_t, node);
  if(node->parent == NULL) return Qnil;
  VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::ASTNode"), NULL, NULL, node->parent);
	rb_obj_call_init(tdata, 0, NULL);
  return tdata;
}

VALUE 
rbc_ast_node_child(VALUE self) {
  ast_node_t *node;
  Data_Get_Struct(self, ast_node_t, node);
  if(node->child == NULL) return Qnil;
  VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::ASTNode"), NULL, NULL, node->child);
	rb_obj_call_init(tdata, 0, NULL);
  return tdata;
}

VALUE
rbc_ast_node_equals(VALUE ra, VALUE rb) {
  ast_node_t *a;
  Data_Get_Struct(ra, ast_node_t, a);
  ast_node_t *b;
  Data_Get_Struct(rb, ast_node_t, b);
  return rb_equal(((value_holder *)a->data)->val, ((value_holder *)b->data)->val);
}

VALUE
rbc_ast_node_insert_before(VALUE class, VALUE self, VALUE other) {
  (void) class;
  ast_node_t *s;
  Data_Get_Struct(self, ast_node_t, s);
  ast_node_t *o;
  Data_Get_Struct(other, ast_node_t, o);
  ast_node_insert_before(s, o);
  return Qnil;
}

VALUE
rbc_ast_node_insert_after(VALUE class, VALUE self, VALUE other) {
  (void) class;
  ast_node_t *s;
  Data_Get_Struct(self, ast_node_t, s);
  ast_node_t *o;
  Data_Get_Struct(other, ast_node_t, o);
  ast_node_insert_after(s, o);
  return Qnil;
}

VALUE
rbc_ast_node_prepend_child(VALUE class, VALUE self, VALUE other) {
  (void) class;
  ast_node_t *s;
  Data_Get_Struct(self, ast_node_t, s);
  ast_node_t *o;
  Data_Get_Struct(other, ast_node_t, o);
  ast_node_prepend_child(s, o);
  return Qnil;
}

VALUE
rbc_ast_node_remove(VALUE class, VALUE self) {
  (void) class;
  ast_node_t *s;
  Data_Get_Struct(self, ast_node_t, s);
  ast_node_remove(s);
  return Qnil;
}

VALUE 
rbc_ast_node_to_s(VALUE self) {
  ast_node_t *node;
  Data_Get_Struct(self, ast_node_t, node);
  VALUE inner = ((value_holder *)node->data)->val;
  return rb_funcall(inner, rb_intern("to_s"), 0);
}


VALUE
rbc_ast_node_wrap(VALUE class, VALUE self, VALUE other) {
  (void) class;
  ast_node_t *s;
  Data_Get_Struct(self, ast_node_t, s);
  ast_node_t *o;
  Data_Get_Struct(other, ast_node_t, o);
  ast_node_wrap(s, o);
  return Qnil;
}
