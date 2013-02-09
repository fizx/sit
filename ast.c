#include "util.h"
#include "ast.h"
#include <stdlib.h>

ast_t *
ast_new(void *(*fmake)(), void (*ffree)(void *)) {
  ast_t *ast = malloc(sizeof(ast_t));
  ast->make_data = fmake;
  ast->free_data = ffree;
  return ast;
}

ast_node_t *
ast_node_new(ast_t *ast) {
  ast_node_t *node = malloc(sizeof(ast_node_t));
  node->parent = NULL;
  node->child = NULL;
  node->next = NULL;
  node->prev = NULL;
  node->ast = ast;
  node->internal = ast->nodes;
  ast->nodes = node;
  if(ast->make_data != NULL) {
    node->data = ast->make_data();
  }
  return node;
}

int
ast_node_child_count(ast_node_t *self) {
	ast_node_t *child = self->child;
	int i = 0;
	while(child) {
		child = child->next;
		i++;
	}
	return i;
}

void
ast_node_replace(ast_node_t *self, ast_node_t *other) {
  ast_node_insert_before(self, other);
  ast_node_remove(self);
}

void
ast_node_insert_before(ast_node_t *self, ast_node_t *other) {
  ast_node_remove(other);

  other->prev = self->prev;
  self->prev = other;
  other->next = self;
  other->parent = self->parent;

  if(other->parent && other->parent->child == self) other->parent->child = other;
  if(other->prev) other->prev->next = other;
}

void
ast_node_insert_after(ast_node_t *self, ast_node_t *other) {
  ast_node_remove(other);

  other->next = self->next;
  self->next = other;
  other->prev = self;
  other->parent = self->parent;

  if(other->next) other->next->prev = other;
}

void
ast_node_prepend_child(ast_node_t *self, ast_node_t *other) {
  ast_node_remove(other);

  ast_node_t *child = self->child;
  if(child == NULL) {
    self->child = other;
    other->parent = self;
  } else {
    ast_node_insert_before(self->child, other);
  }
}

void
ast_node_append_child(ast_node_t *self, ast_node_t *other) {
  ast_node_remove(other);

  ast_node_t *child = self->child;
  if(child == NULL) {
    self->child = other;
    other->parent = self;
  } else {
    while(child->next) {
      child = child->next;
    }
    ast_node_insert_after(child, other);
  }  
}

void
ast_node_remove(ast_node_t *self) {
  if(self->parent && self->parent->child == self) {
    self->parent->child = self->next;
  }
  if(self->prev) self->prev->next = self->next;
  if(self->next) self->next->prev = self->prev;

  self->parent = NULL;
  self->prev = NULL;
  self->next = NULL;
}

void
ast_node_wrap(ast_node_t *self, ast_node_t *other) {
  other->prev = self->prev;
  other->next = self->next;
  other->parent = self->parent;

  if(other->prev) other->prev->next = other;
  if(other->next) other->next->prev = other;
  if(other->parent->child == self) other->parent->child = other;
  
  ast_node_prepend_child(other, self);
}

void
ast_node_unwrap(ast_node_t *self) {
  while(self->child) {
    ast_node_insert_before(self, self->child);
  }
  ast_node_remove(self);
}
