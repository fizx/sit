#include "sit.h"

AST *
ast_new(void *(*fmake)(), void (*ffree)(void *)) {
  AST *ast = calloc(1, sizeof(AST));
  ast->make_data = fmake;
  ast->free_data = ffree;
  ast->nodes = NULL;
  ast->freeable = NULL;
  return ast;
}

void
ast_free(AST *ast) {
  ASTNode *node = ast->nodes;
  ASTNode *tmp;
  while(node) {
    if(ast->free_data) {
      ast->free_data(node->data);
    }
    tmp = node;
    node = node->internal;
    free(tmp);
  }
  ll_free(ast->freeable);
  free(ast);
}

ASTNode *
ast_node_new(AST *ast) {
  ASTNode *node = calloc(1, sizeof(ASTNode));
  node->ast = ast;
  node->internal = ast->nodes;
  ast->nodes = node;
  if(ast->make_data != NULL) {
    node->data = ast->make_data();
  }
  return node;
}

int
ast_node_child_count(ASTNode *self) {
	ASTNode *child = self->child;
	int i = 0;
	while(child) {
		child = child->next;
		i++;
	}
	return i;
}

void
ast_node_replace(ASTNode *self, ASTNode *other) {
  ast_node_insert_before(self, other);
  ast_node_remove(self);
}

void
ast_node_insert_before(ASTNode *self, ASTNode *other) {
  ast_node_remove(other);

  other->prev = self->prev;
  self->prev = other;
  other->next = self;
  other->parent = self->parent;

  if(other->parent && other->parent->child == self) other->parent->child = other;
  if(other->prev) other->prev->next = other;
}

void
ast_node_insert_after(ASTNode *self, ASTNode *other) {
  ast_node_remove(other);

  other->next = self->next;
  self->next = other;
  other->prev = self;
  other->parent = self->parent;

  if(other->next) other->next->prev = other;
}

void
ast_node_prepend_child(ASTNode *self, ASTNode *other) {
  ast_node_remove(other);

  ASTNode *child = self->child;
  if(child == NULL) {
    self->child = other;
    other->parent = self;
  } else {
    ast_node_insert_before(self->child, other);
  }
}

void
ast_node_append_child(ASTNode *self, ASTNode *other) {
  ast_node_remove(other);

  ASTNode *child = self->child;
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
ast_node_remove(ASTNode *self) {
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
ast_node_wrap(ASTNode *self, ASTNode *other) {
  other->prev = self->prev;
  other->next = self->next;
  other->parent = self->parent;

  if(other->prev) other->prev->next = other;
  if(other->next) other->next->prev = other;
  if(other->parent->child == self) other->parent->child = other;
  
  ast_node_prepend_child(other, self);
}

void
ast_node_unwrap(ASTNode *self) {
  while(self->child) {
    ast_node_insert_before(self, self->child);
  }
  ast_node_remove(self);
}
