#ifndef AST_RUBY_H_INCLUDED
#define AST_RUBY_H_INCLUDED

#include "ruby.h"

VALUE 
rbc_ast_new(VALUE class);

VALUE 
rbc_ast_node_new(VALUE class, VALUE ast, VALUE rdata);

VALUE
rbc_ast_node_get_data(VALUE self);

VALUE
rbc_ast_node_set_data(VALUE self, VALUE rdata);

VALUE 
rbc_ast_node_next(VALUE self);

VALUE 
rbc_ast_node_prev(VALUE self);

VALUE 
rbc_ast_node_parent(VALUE self);

VALUE 
rbc_ast_node_child(VALUE self);

VALUE
rbc_ast_node_equals(VALUE self, VALUE other);

VALUE
rbc_ast_node_insert_before(VALUE class, VALUE self, VALUE other);

VALUE
rbc_ast_node_insert_after(VALUE class, VALUE self, VALUE other);

VALUE
rbc_ast_node_prepend_child(VALUE class, VALUE self, VALUE other);

VALUE
rbc_ast_node_remove(VALUE class, VALUE self);

VALUE
rbc_ast_node_wrap(VALUE class, VALUE self, VALUE other);

VALUE 
rbc_ast_node_to_s(VALUE self);

#endif
