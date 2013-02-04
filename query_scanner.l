%option reentrant
%option bison-bridge
%option bison-locations
%option noyywrap
%option yylineno

D			[0-9]
L			[a-zA-Z_]
H			[a-fA-F0-9]
NS    [^ \t\n\r,?*();:\^"]

%{
#include "y.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "query_parser.h"
#include "white_parser.h"
#include "pstring.h"
#include <assert.h>

#define YY_EXTRA_TYPE query_parser*
#define YY_USER_ACTION yylloc->first_line = yylineno;

#define YY_INPUT(cbuf, offset, max_size)  {                                   \
  int i;\
  for(i = 0; i < max_size && offset < yyextra->buf->len; i++, offset++) { \
    cbuf[i] = yyextra->buf->val[offset];                                      \
  }                                                                         \
  if(i == 0) {                                                              \
    yyextra->done++;                                                   \
  }                                                                         \
  offset = i;                                                                 \
}                                                                             

%}

%%

("and"|"AND")             { return(AND); }
("or"|"OR")               { return(OR); }
("not"|"NOT")             { return(NOT); }
"("                       { return(LPAREN); }
")"                       { return(RPAREN); }
"="                       { return(EQ); }
">"                       { return(GT); }
"<"                       { return(LT); }
">="                      { return(GTE); }
"<="                      { return(LTE); }
"~"                       { return(TILDE); }
"<>"                      { return(NEQ); }
"-"                       { return(MINUS); }
{D}+                      { yyextra->ptr = cstring_new(yytext, yyleng); return(DIGITS); }
"."                       { return(DOT); }
L?\"(\\.|[^\\"])*\"	      { yyextra->ptr = cstring_new(yytext, yyleng); return(STRING_LITERAL); }
{NS}+             	      { yyextra->ptr = cstring_new(yytext, yyleng); return(UNQUOTED); }
[;\n]+                    { return(EOQ); }
[ \t\v\f]  		            { /* ignore whitespace*/ }
.			                    { /* ignore bad characters */ }

%%

ast_node_t *
query_node_new(query_parser *qp, query_node_type type) {
  query_node *node = malloc(sizeof(query_node));
  node->type     = type;
  node->field    = NULL;
  node->val      = NULL;
  node->num      = 0;
  node->negated  = false;
  
  ast_node_t *ast_node = ast_node_new(qp->ast);
  ast_node->data = node;
  
  return ast_node;
}

query_parser *
query_parser_new(sit_callback *cb) {
  query_parser *parser = malloc(sizeof(query_parser));
  parser->buf = NULL;
  parser->error = NULL;
  parser->ast = ast_new(NULL, free);
  parser->root = NULL;
  parser->cb = cb;
  parser->tokenizer = white_parser_new();
  parser->push_state = yypstate_new();
  parser->lvalp = malloc(sizeof(YYSTYPE));
  parser->llocp = malloc(sizeof(YYLTYPE));
  yylex_init(&parser->scanner);
  yyset_extra(parser, parser->scanner);
  return parser;
}

int
query_parser_consume(query_parser *parser, pstring *pstr) {
  assert(pstr);
  assert(pstr->len);
  parser->buf = pstr;
  parser->done = false;
  //return yyparse(parser);
  int status = YYPUSH_MORE;
  while (status == YYPUSH_MORE && parser->done < 1) {
    status = yypush_parse(
      parser->push_state,
      yylex(parser->lvalp, parser->llocp, parser->scanner), 
      parser->lvalp,
      parser->llocp,
      parser);
  }
  return status;
}

#define Q(node)     ((query_node *)(node->data))
#define NEXT(obj)   (obj ? obj->next : NULL)
void 
associate_ands(query_parser *context, ast_node_t *node) {
  if(!node) return;
  ast_node_t *op    = node->next;
  ast_node_t *other = NEXT(op);
  
  if(op && Q(op)->type == BAND) {
    ast_node_t *wrapper = query_node_new(context, ANDS); 
    ast_node_insert_before(op, wrapper);
    ast_node_prepend_child(wrapper, other);
    ast_node_prepend_child(wrapper, node);
    ast_node_remove(op);
    associate_ands(context, wrapper);
  }
  associate_ands(context, other);
  associate_ands(context, node->child);
}

void 
combine_ors(query_parser *context, ast_node_t *node) {
  if(!node) return;
  ast_node_t *op    = node->next;
  ast_node_t *other = NEXT(op);
  
  if(op && Q(op)->type == BOR) {
    ast_node_t *wrapper = query_node_new(context, ORS); 
    ast_node_insert_before(op, wrapper);
    ast_node_prepend_child(wrapper, other);
    ast_node_prepend_child(wrapper, node);
    ast_node_remove(op);
    combine_ors(context, wrapper);
  }
  combine_ors(context, op);
  combine_ors(context, node->child);
}

void
demorgans(query_parser *context, ast_node_t *node) {
  if(!node) return;
  ast_node_t *tmp;
  
  if(Q(node)->negated) {
    switch(Q(node)->type) {
    case EXPR   :
      Q(node)->negated = false;
      tmp = node->child;
      while(tmp) {
        Q(tmp)->negated = !Q(tmp)->negated;
        tmp = tmp->next;
      }
      break;
    case ANDS   :
      Q(node)->negated = false;
      Q(node)->type = ORS;
      tmp = node->child;
      while(tmp) {
        Q(tmp)->negated = !Q(tmp)->negated;
        tmp = tmp->next;
      }
      break;
    case ORS    :
      Q(node)->negated = false;
      Q(node)->type = ANDS;
      tmp = node->child;
      while(tmp) {
        Q(tmp)->negated = !Q(tmp)->negated;
        tmp = tmp->next;
      }
      break;
    case CLAUSE :
    case TERM:
    case NUMTERM :
      break;
    case UNKNOWN:
    case NUM    :
    case CMP    :
    case STR    :
    case MODSTR :
    case BAND   :
    case BOR    :
      assert(0);
    }
  }
  
  demorgans(context, node->next);
  demorgans(context, node->child);
}


void
unwrap_exprs(query_parser *context, ast_node_t *node) {
  if(!node) return;
  unwrap_exprs(context, node->next);
  unwrap_exprs(context, node->child);
  if(node && Q(node)->type == EXPR) {
    ast_node_unwrap(node);
  }
}

void 
merge_bools(query_parser *context, ast_node_t *node, query_node_type type) {
  if(!node) return;
  merge_bools(context, node->next, type);
  merge_bools(context, node->child, type);
  if(Q(node)->type == type) {
    ast_node_t *child = node->child;
    int count = 0;
    while(child) {
      if(Q(child)->type == type) {
        ast_node_unwrap(child);
        child = node->child;
        int count = 0;
      } else {
        count++;
        child = child->next;
      }
    }
    if(count == 1) {
      ast_node_unwrap(node);
    }
  }
}

void 
bubble_ors(query_parser *context, ast_node_t *node) {
  if(!node) return;

  ast_node_t *parent = node->parent;
  if(Q(node)->type == ORS && Q(parent)->type != EXPR) {
    ast_node_t *gp = query_node_new(context, ORS);
    ast_node_t *child = node->child;
    for (int i = 0; child; i++, child = child->next) {
      ast_node_t *cp = query_node_copy_subtree(context, parent);
      ast_node_append_child(gp, cp);
      ast_node_t *cpkid = cp->child;
      while(cpkid) {
        if(Q(cpkid)->type == ORS) {
          ast_node_insert_before(cpkid, query_node_copy_subtree(context, child));
          ast_node_remove(cpkid);
          break;
        }
        cpkid = cpkid->next;
      }
    }
    ast_node_insert_before(parent, gp);
    ast_node_remove(parent);
  }
  
  
  bubble_ors(context, node->child); // depth-first
  bubble_ors(context, node->next); // left-to-right
}

void
add_token(sit_parser *parser, long off, int len, int field_offset) {
  query_parser *context = parser->data;
  ast_node_t *node = context->tmp;
  pstring *field = Q(node)->field;
  pstring *val = Q(node)->val;
  pstring *pstr = pstring_new2(val->val + off, len);
  ast_node_t *term = query_node_new(context, TERM);
  Q(term)->cmp = Q(node)->cmp;
  Q(term)->field = field;
  Q(term)->val = pstr;
  ast_node_append_child(node, term);
}
  

void
expand_clauses(query_parser *context, ast_node_t *node) {
  if(!node) return;

  if(Q(node)->type == CLAUSE) {
    ast_node_t *field = node->child;
    ast_node_t *op  = node->child->next;
    ast_node_t *val = node->child->next->next;
    if(Q(op)->cmp == _TILDE && Q(val)->type == STR) {
      Q(node)->type = ANDS;
      context->tokenizer->data = context;
      context->tokenizer->term_found = add_token;
      context->tmp = node;
      Q(node)->field = Q(field)->val;
      Q(node)->cmp = Q(op)->cmp;
      Q(node)->val = Q(val)->val;
      ast_node_remove(field);
      ast_node_remove(op);
      ast_node_remove(val);
      context->tokenizer->consume(context->tokenizer, Q(node)->val);
    } else if(Q(op)->cmp != _TILDE && Q(val)->type == NUM) {
      Q(node)->type = NUMTERM;
      node->child = NULL;
    } else {
      assert(0);
    }
  }
  expand_clauses(context, node->child); 
  expand_clauses(context, node->next);
}

void
query_parser_construct(query_parser *context, ast_node_t *expression) {
  context->root = expression;
  // pstring *pstr = query_node_query(expression);
  // printf("%.*s\n", pstr->len, pstr->val);
  // pstr = query_node_ast_to_s(expression);
  // printf("\n%.*s\n****************\n", pstr->len, pstr->val);
  expand_clauses(context, expression);
  associate_ands(context, expression);
  combine_ors(context, expression);
  demorgans(context, expression);
  unwrap_exprs(context, expression->child);
  merge_bools(context, expression, ORS);
  merge_bools(context, expression, ANDS);
  bubble_ors(context, expression);
  merge_bools(context, expression, ORS);
  merge_bools(context, expression, ANDS);
  // pstr = query_node_query(expression);
  // printf("%.*s\n", pstr->len, pstr->val);
  // pstr = query_node_ast_to_s(expression);
  // printf("%.*s\n", pstr->len, pstr->val);
}

char *
_s(query_node_type t) {
  switch(t) {
    case UNKNOWN : return "UNKNOWN" ;
    case NUM     : return "NUM"     ;
    case EXPR    : return "EXPR"    ;
    case CLAUSE  : return "CLAUSE"  ;
    case CMP     : return "CMP"     ;
    case STR     : return "STR"     ;
    case MODSTR  : return "MODSTR"  ;
    case BAND    : return "BAND"    ;
    case BOR     : return "BOR"     ;
    case ANDS    : return "ANDS"    ;
    case ORS     : return "ORS"     ;
    case TERM    : return "TERM"    ;
    case NUMTERM : return "NUMTERM" ;
  }
}

char *
_c(cmp_type t) {
  switch(t) {
    case _NA    : return "??";
    case _EQ    : return "=";
    case _GT    : return ">";
    case _LT    : return "<";
    case _GTE   : return ">=";
    case _LTE   : return "<=";
    case _TILDE : return "~";
    case _NEQ   : return "!=";
    default     : return "??";
  }
}

#define P(x)   padd(buf, x)
#define PC(x)   paddc(buf, x)
#define PQ(x)  padd(buf, query_node_query(x))

pstring * 
query_node_query(ast_node_t *node) {
  char *tmp;
  pstring *buf = pstring_new(0);
  if(Q(node)->negated) PC("NOT ");
  switch(Q(node)->type) {
  case EXPR:
    PQ(node->child);
    break;
  case CLAUSE:
    node = node->child;
    while(node) {
      PQ(node);
      node = node->next;
    }
    break;
  case CMP:
    PC(" ");
    PC(_c(Q(node)->cmp));
    PC(" ");
    break;
  case STR:
    P(Q(node)->val);
    break;
  case TERM:
    assert(Q(node)->field);
    assert(Q(node)->val);
    asprintf(&tmp, "%.*s %s %.*s", Q(node)->field->len, Q(node)->field->val, _c(Q(node)->cmp), Q(node)->val->len, Q(node)->val->val);
    PC(tmp);
    free(tmp);
    break;
  case NUM:
    asprintf(&tmp, "%d", Q(node)->num);
    PC(tmp);
    free(tmp);
    break;
  case MODSTR:
    P(Q(node)->val);
    PC("(");
    PQ(node->next);
    PC(")");
    break;
  case ANDS:
    tmp = "first";
    node = node->child;
    PC("(");
    while(node) {
      if(!tmp) PC(" AND ");
      tmp = NULL;
      PQ(node);
      node = node->next;
    }
    PC(")");
    break;
  case ORS:
    tmp = "first";
    node = node->child;
    PC("(");
    while(node) {
      if(!tmp) PC(" OR ");
      tmp = NULL;
      PQ(node);
      node = node->next;
    }
    PC(")");
    break;
  }
  return buf;
}

pstring * 
_query_node_ast_to_s(ast_node_t *node, int level) {
  pstring *buf = pstring_new(0);
  if(!node) return buf;
  assert(node->ast);
  for(int i = 0; i < level; i++) PC("\t");
  P(query_node_to_s(Q(node)));
  PC("\n");
  P(_query_node_ast_to_s(node->child, level + 1));
  P(_query_node_ast_to_s(node->next, level));
  return buf;
}

pstring * 
query_node_ast_to_s(ast_node_t *node) {
  return _query_node_ast_to_s(node, 0);
}

pstring * 
query_node_to_s(query_node *node) {
  char *str;
  int len = 4;
  char *val = "NULL";
  if(node->val) {
    len = node->val->len;
    val = node->val->val;
  }
  asprintf(&str, "[%s %.*s %d %s %s]", _s(node->type), len, val, node->num, _c(node->cmp), node->negated ? "true" : "false");
  pstring *pstr = malloc(sizeof(*pstr));
  pstr->val = str;
  pstr->len = strlen(str);
  return pstr;
}