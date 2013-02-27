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

#include "sit.h"

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
("limit"|"LIMIT")         { return(LIMIT); }
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

ASTNode *
query_node_new(query_parser *qp, query_node_type type) {
  query_node *node = malloc(sizeof(query_node));
  node->type     = type;
  node->field    = NULL;
  node->val      = NULL;
  node->num      = 0;
  node->negated  = false;
  
  ASTNode *ast_node = ast_node_new(qp->ast);
  ast_node->data = node;
  
  return ast_node;
}

query_parser *
query_parser_new(Callback *cb) {
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

void
query_parser_reset(query_parser *parser) {
  parser->push_state = yypstate_new();
  yylex_init(&parser->scanner);
  yyset_extra(parser, parser->scanner);
}

int
query_parser_consume(query_parser *parser, pstring *pstr) {
  assert(pstr);
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
associate_ands(query_parser *context, ASTNode *node) {
  if(!node) return;
  ASTNode *op    = node->next;
  ASTNode *other = NEXT(op);
  
  if(op && Q(op)->type == BAND) {
    ASTNode *wrapper = query_node_new(context, ANDS); 
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
combine_ors(query_parser *context, ASTNode *node) {
  if(!node) return;
  ASTNode *op    = node->next;
  ASTNode *other = NEXT(op);
  
  if(op && Q(op)->type == BOR) {
    ASTNode *wrapper = query_node_new(context, ORS); 
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
demorgans(query_parser *context, ASTNode *node) {
  if(!node) return;
  ASTNode *tmp;
  
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
unwrap_exprs(query_parser *context, ASTNode *node) {
  if(!node) return;
  unwrap_exprs(context, node->next);
  unwrap_exprs(context, node->child);
  if(node && Q(node)->type == EXPR) {
    ast_node_unwrap(node);
  }
}

void 
merge_bools(query_parser *context, ASTNode *node, query_node_type type) {
  if(!node) return;
  merge_bools(context, node->next, type);
  merge_bools(context, node->child, type);
  if(Q(node)->type == type) {
    ASTNode *child = node->child;
    int count = 0;
    while(child) {
      if(Q(child)->type == type) {
        ast_node_unwrap(child);
        child = node->child;
        count = 0;
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
bubble_ors(query_parser *context, ASTNode *node) {
  if(!node) return;

  ASTNode *parent = node->parent;
  if(Q(node)->type == ORS && Q(parent)->type != EXPR) {
    ASTNode *gp = query_node_new(context, ORS);
    ASTNode *child = node->child;
    for (int i = 0; child; i++, child = child->next) {
      ASTNode *cp = query_node_copy_subtree(context, parent);
      ast_node_append_child(gp, cp);
      ASTNode *cpkid = cp->child;
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
add_token(Receiver *receiver, pstring *val, int field_offset) {
  query_parser *context = receiver->data;
  ASTNode *node = context->tmp;
  pstring *field = Q(node)->field;
  Q(node)->num = field_offset;
  ASTNode *term = query_node_new(context, TERM);
  Q(term)->cmp = Q(node)->cmp;
  Q(term)->field = field;
  Q(term)->val = val;
  ast_node_append_child(node, term);
}

Receiver token_receiver = {
  add_token,
  NULL,
  NULL,
  NULL,
  NULL,
	NULL
};

void
expand_clauses(query_parser *context, ASTNode *node) {
  if(!node) return;

  if(Q(node)->type == CLAUSE) {
    ASTNode *field = node->child;
    ASTNode *op  = node->child->next;
    ASTNode *val = node->child->next->next;
    if(Q(op)->cmp == _TILDE && Q(val)->type == STR) {
      Q(node)->type = ANDS;
      context->tokenizer->receiver = &token_receiver;
      context->tokenizer->receiver->data = context;
      context->tmp = node;
      Q(node)->field = Q(field)->val;
      Q(node)->cmp = Q(op)->cmp;
      Q(node)->val = Q(val)->val;
      ast_node_remove(field);
      ast_node_remove(op);
      ast_node_remove(val);
      context->tokenizer->consume(context->tokenizer, Q(node)->val);
      context->tokenizer->end_stream(context->tokenizer);
    } else if(Q(op)->cmp != _TILDE && Q(val)->type == NUM) {
      Q(node)->type = NUMTERM;
      Q(node)->field = Q(field)->val;
      Q(node)->cmp = Q(op)->cmp;
      Q(node)->num = Q(val)->num;
      node->child = NULL;
    } else {
      assert(0);
    }
  }
  expand_clauses(context, node->child); 
  expand_clauses(context, node->next);
}

void *
to_conjunction(ASTNode *node, void *obj) {
  switch(Q(node)->type) {
  case NUMTERM:
  case TERM: {
    conjunction_t *con = malloc(sizeof(conjunction_t));
    memcpy(&con->terms[0], obj, sizeof(sit_term));
  	con->count = 1;
  	con->data = NULL;
    return con;
  }
  case ANDS: 
    return obj;
  default:
    assert(0);
  }
}

void *
to_query(ASTNode *node, void *obj) {
  switch(Q(node)->type) {
  case NUMTERM:
  case TERM: {
    obj = to_conjunction(node, obj);
    // fall-through
  }
  case ANDS: {
    Query *query = malloc(sizeof(Query));
    query->count = 1;
    query->conjunctions = malloc(sizeof(conjunction_t*));
    query->conjunctions[0] = obj;
    return query;
  }
  case ORS: 
    return obj;
  default:
    assert(0);
  }
}

pstring _na = { "?", 1 };
pstring _eq = { "=", 1 };
pstring _gt = { ">", 1 };
pstring _lt = { "<", 1 };
pstring _gte = { ".", 1 };
pstring _lte = { ",", 1 };
pstring _tilde = { "~", 1 };
pstring _neq = { "!", 1 };

pstring * 
cmpmap(cmp_type t) {
  switch(t) {
       case _NA: return &_na;
       case _EQ: return &_eq;
       case _GT: return &_gt;
       case _LT: return &_lt;
       case _GTE: return &_gte;
       case _LTE: return &_lte;
       case _TILDE: return &_tilde;
       case _NEQ: return &_neq;
     }
  return NULL;
}

pstring * 
_negate(pstring *t) {
  switch(*t->val) {
    case '?': return &_na;
    case '=': return &_neq;
    case '>': return &_lte;
    case '<': return &_gte;
    case '.': return &_lt;
    case ',': return &_gt;
    case '~': return &_na;
    case '!': return &_eq;
  }
  return &_na;
}

void *
make_query_and_callback(query_parser *context, ASTNode *node) {
  switch(Q(node)->type) {
  case NUMTERM: {
    pstring *c = cmpmap(Q(node)->cmp);
    if(Q(node)->negated) c = _negate(c);
    sit_term *term = sit_term_new(Q(node)->field, c, Q(node)->num, false);
    term->numeric = true;
    return term;
  }
  case TERM:
    return sit_term_new(Q(node)->field, Q(node)->val, Q(node)->num, Q(node)->negated);
  case ANDS: {
    int count = ast_node_child_count(node);
    conjunction_t *con = malloc(sizeof(conjunction_t) + (count - 1) * sizeof(sit_term));
    ASTNode *child = node->child;
  	for(int i = 0	; i < count; i++) {
  		memcpy(&con->terms[i], make_query_and_callback(context, child), sizeof(sit_term));
  		child = child->next;
  	}
  	qsort(con->terms, count, sizeof(sit_term), qsit_termcmp);	
  	con->count = count;
  	con->data = NULL;
  	return con;
  } 
  case ORS: {
    int count = ast_node_child_count(node);
    Query *query = malloc(sizeof(Query));
    query->count = count;
    query->conjunctions = malloc(count * sizeof(conjunction_t*));
    ASTNode *child = node->child;
    for(int i = 0	; i < count; i++) {
      query->conjunctions[i] = to_conjunction(child, make_query_and_callback(context, child));
  		child = child->next;
  	}
  } 
  case EXPR: {
    return to_query(node->child, make_query_and_callback(context, node->child));
  }
  default:
    assert(0);
  }
}

void
query_parser_construct(query_parser *context, ASTNode *expression, int limit) {
  pstring *pstr;
  (void) pstr;
  context->root = expression;
  // pstr = query_node_query(expression);
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
  
  Query * query = make_query_and_callback(context, expression);
  query->limit = limit;
  context->cb->handler(context->cb, query);
}

const char *
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

const char *
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

#define PQ(x)  padd(buf, query_node_query(x))

pstring * 
query_node_query(ASTNode *node) {
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
    PV("%.*s %s %.*s", Q(node)->field->len, Q(node)->field->val, _c(Q(node)->cmp), Q(node)->val->len, Q(node)->val->val);
    break;
  case NUM:
    PV("%d", Q(node)->num);
    break;
  case MODSTR:
    P(Q(node)->val);
    PC("(");
    PQ(node->next);
    PC(")");
    break;
  case ANDS: {
    bool first = true;
    node = node->child;
    PC("(");
    while(node) {
      if(!first) PC(" AND ");
      first = false;
      PQ(node);
      node = node->next;
    }
    PC(")");
    break;
  }
  case ORS: {
    bool first = true;
    node = node->child;
    PC("(");
    while(node) {
      if(!first) PC(" OR ");
      first = false;
      PQ(node);
      node = node->next;
    }
    PC(")");
    break;
  }
  }
  return buf;
}

pstring * 
_query_node_ast_to_s(ASTNode *node, int level) {
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
query_node_ast_to_s(ASTNode *node) {
  return _query_node_ast_to_s(node, 0);
}

pstring * 
query_node_to_s(query_node *node) {
  pstring *buf = pstring_new(0);
  int len = 4;
  const char *val = "NULL";
  if(node->val) {
    len = node->val->len;
    val = node->val->val;
  }
  PV("[%s %.*s %d %s %s %s]", _s(node->type), len, val, node->num, _c(node->cmp), node->negated ? "true" : "false");
  return buf;
}