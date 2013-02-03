#ifndef AST_H_INCLUDED
#define AST_H_INCLUDED

struct ast_s;

typedef struct ast_node_s {
  struct ast_s      *ast;
  struct ast_node_s *child;
  struct ast_node_s *parent;
  struct ast_node_s *prev;
  struct ast_node_s *next;
  void *data;
  void *internal;
} ast_node_t;

typedef struct ast_s {
  void *(*make_data)();
  void  (*free_data)(void *data);
  ast_node_t *nodes;
} ast_t;


ast_t *
ast_new(void *(*fmake)(), void (*ffree)(void *));

ast_node_t *
ast_node_new(ast_t *ast); 

void
ast_node_insert_before(ast_node_t *self, ast_node_t *other);

void
ast_node_insert_after(ast_node_t *self, ast_node_t *other);

void
ast_node_prepend_child(ast_node_t *self, ast_node_t *other);

void
ast_node_append_child(ast_node_t *self, ast_node_t *other);

void
ast_node_remove(ast_node_t *self);

void
ast_node_wrap(ast_node_t *self, ast_node_t *other);

void
ast_node_unwrap(ast_node_t *self);

#endif
