#include "sit_ruby.h"

void *
wrapnil() {
  return vwrap(Qnil);
}

VALUE 
rbc_ast_new(VALUE class) {	
  AST *ast = ast_new(wrapnil, free);
	VALUE tdata = Data_Wrap_Struct(class, NULL, ast_free, ast);
	rb_obj_call_init(tdata, 0, NULL);
  return tdata;
}

VALUE 
rbc_ast_node_new(VALUE class, VALUE rast, VALUE rdata) {
  AST *ast;
  Data_Get_Struct(rast, AST, ast);
  ASTNode *node = ast_node_new(ast);  
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, node);
	rb_obj_call_init(tdata, 0, NULL);
  rbc_ast_node_set_data(tdata, rdata);
  return tdata;
}

VALUE
rbc_ast_node_get_data(VALUE self) {
  ASTNode *node;
  Data_Get_Struct(self, ASTNode, node);
  return vunwrap(node->data);
}

VALUE
rbc_ast_node_set_data(VALUE self, VALUE rdata) {
  ASTNode *node;
  Data_Get_Struct(self, ASTNode, node);
  ((value_holder *)node->data)->val = rdata;
  return rdata;
}

VALUE 
rbc_ast_node_next(VALUE self) {
  ASTNode *node;
  Data_Get_Struct(self, ASTNode, node);
  if(node->next == NULL) return Qnil;
  VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::ASTNode"), markall, NULL, node->next);
	rb_obj_call_init(tdata, 0, NULL);
  return tdata;
}

VALUE 
rbc_ast_node_prev(VALUE self) {
  ASTNode *node;
  Data_Get_Struct(self, ASTNode, node);
  if(node->prev == NULL) return Qnil;
  VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::ASTNode"), markall, NULL, node->prev);
	rb_obj_call_init(tdata, 0, NULL);
  return tdata;
}

VALUE 
rbc_ast_node_parent(VALUE self) {
  ASTNode *node;
  Data_Get_Struct(self, ASTNode, node);
  if(node->parent == NULL) return Qnil;
  VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::ASTNode"), markall, NULL, node->parent);
	rb_obj_call_init(tdata, 0, NULL);
  return tdata;
}

VALUE 
rbc_ast_node_child(VALUE self) {
  ASTNode *node;
  Data_Get_Struct(self, ASTNode, node);
  if(node->child == NULL) return Qnil;
  VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::ASTNode"), markall, NULL, node->child);
	rb_obj_call_init(tdata, 0, NULL);
  return tdata;
}

VALUE
rbc_ast_node_equals(VALUE ra, VALUE rb) {
  ASTNode *a;
  Data_Get_Struct(ra, ASTNode, a);
  ASTNode *b;
  Data_Get_Struct(rb, ASTNode, b);
  return rb_equal(((value_holder *)a->data)->val, ((value_holder *)b->data)->val);
}

VALUE
rbc_ast_node_insert_before(VALUE class, VALUE self, VALUE other) {
  (void) class;
  ASTNode *s;
  Data_Get_Struct(self, ASTNode, s);
  ASTNode *o;
  Data_Get_Struct(other, ASTNode, o);
  ast_node_insert_before(s, o);
  return Qnil;
}

VALUE
rbc_ast_node_insert_after(VALUE class, VALUE self, VALUE other) {
  (void) class;
  ASTNode *s;
  Data_Get_Struct(self, ASTNode, s);
  ASTNode *o;
  Data_Get_Struct(other, ASTNode, o);
  ast_node_insert_after(s, o);
  return Qnil;
}

VALUE
rbc_ast_node_prepend_child(VALUE class, VALUE self, VALUE other) {
  (void) class;
  ASTNode *s;
  Data_Get_Struct(self, ASTNode, s);
  ASTNode *o;
  Data_Get_Struct(other, ASTNode, o);
  ast_node_prepend_child(s, o);
  return Qnil;
}

VALUE
rbc_ast_node_remove(VALUE class, VALUE self) {
  (void) class;
  ASTNode *s;
  Data_Get_Struct(self, ASTNode, s);
  ast_node_remove(s);
  return Qnil;
}

VALUE 
rbc_ast_node_to_s(VALUE self) {
  ASTNode *node;
  Data_Get_Struct(self, ASTNode, node);
  VALUE inner = ((value_holder *)node->data)->val;
  return rb_funcall(inner, rb_intern("to_s"), 0);
}


VALUE
rbc_ast_node_wrap(VALUE class, VALUE self, VALUE other) {
  (void) class;
  ASTNode *s;
  Data_Get_Struct(self, ASTNode, s);
  ASTNode *o;
  Data_Get_Struct(other, ASTNode, o);
  ast_node_wrap(s, o);
  return Qnil;
}
