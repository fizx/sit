#include "sit.h"

int
qtermcmp(const void *a, const void *b) {
	const Term *ta = a;
	const Term *tb = b;
  int out = ta->negated - tb->negated;
  if(!out) {
	  out = termcmp(ta, tb);
  }
  return out;
}

Query *
query_new(conjunction_t **conjunctions, int count, Callback *callback) {
  Query *query = malloc(sizeof(*query));
  query->conjunctions = malloc(sizeof(conjunction_t*) * count);
  memcpy(query->conjunctions, conjunctions, sizeof(conjunction_t*) * count);
	query->count = count;
	query->callback = callback;
	query->data = NULL;
	return query;
}

conjunction_t *
conjunction_new(Term **terms, int count) {
  conjunction_t *con = malloc(sizeof(conjunction_t) + (count - 1) * sizeof(Term));
	for(int i = 0	; i < count; i++) {
		memcpy(&con->terms[i], terms[i], sizeof(Term));
	}
	qsort(con->terms, count, sizeof(Term), qtermcmp);	
	con->count = count;
	con->data = NULL;
	return con;
}

void
_conjunction_free(conjunction_t *con) {
  for(int i = 0; i < con->count; i++) {
    Term *term = &con->terms[i];
    if(term->owns_string) {
      free(term->field.val);
  		free(term->text.val);
    }
  }
  free(con);
}

void 
query_free(Query *query) {
  if(query->callback)	callback_free(query->callback);
  for(int i = 0; i < query->count; i++) {
    _conjunction_free(query->conjunctions[i]);
  }
  free(query->conjunctions);
	free(query);
}

pstring *
query_to_s(Query *query) {
	pstring *buf = pstring_new(0);
	PC("<");
	for(int i = 0; i < query->count; i++) {
	  if(i > 0) {
			PC(" OR ");
    }
		conjunction_t *con = query->conjunctions[i];
		P(conjunction_to_s(con));
	}
	PC(" cb:");
	if (query->callback) {
		PV("%d", query->callback->id);
	} else {
		PC("NULL");
	}
	PC(">");
	return buf;
}

pstring *
conjunction_to_s(conjunction_t *con) {
	pstring *buf = pstring_new(0);
	PC("(");
	for(int i = 0; i < con->count; i++) {
	  if(i > 0) {
			PC(" AND ");
    }
		Term *term = &con->terms[i];
		if(term->negated) {
			PC("NOT ");
		}
		P(term_to_s(term));
	}
	PC(")");
	
	return buf;
	
}
