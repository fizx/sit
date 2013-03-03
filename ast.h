#ifndef AST_H_INCLUDED
#define AST_H_INCLUDED

struct AST;

typedef struct ASTNode {
  struct AST     *ast;
  struct ASTNode *child;
  struct ASTNode *parent;
  struct ASTNode *prev;
  struct ASTNode *next;
  void *data;
  void *internal;
} ASTNode;

typedef struct AST {
  void *(*make_data)();
  void  (*free_data)(void *data);
  ASTNode *nodes;
} AST;


AST *
ast_new(void *(*fmake)(), void (*ffree)(void *));

void
ast_free(AST *ast);

ASTNode *
ast_node_new(AST *ast); 

int
ast_node_child_count(ASTNode *self);

void
ast_node_insert_before(ASTNode *self, ASTNode *other);

void
ast_node_insert_after(ASTNode *self, ASTNode *other);

void
ast_node_prepend_child(ASTNode *self, ASTNode *other);

void
ast_node_append_child(ASTNode *self, ASTNode *other);

void
ast_node_remove(ASTNode *self);

void
ast_node_replace(ASTNode *self, ASTNode *other);

void
ast_node_wrap(ASTNode *self, ASTNode *other);

void
ast_node_unwrap(ASTNode *self);

#endif
