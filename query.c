#include "sit.h"

int
qsit_termcmp(const void *a, const void *b) {
	const sit_term *ta = a;
	const sit_term *tb = b;
  int out = ta->negated - tb->negated;
  if(!out) {
	  out = sit_termcmp(ta, tb);
  }
  return out;
}

sit_query *
sit_query_new(conjunction_t **conjunctions, int count, sit_callback *callback) {
  sit_query *query = malloc(sizeof(*query));
  query->conjunctions = malloc(sizeof(conjunction_t*) * count);
  memcpy(query->conjunctions, conjunctions, sizeof(conjunction_t*) * count);
	query->count = count;
	query->callback = callback;
	query->data = NULL;
	return query;
}

conjunction_t *
conjunction_new(sit_term **terms, int count) {
  conjunction_t *con = malloc(sizeof(conjunction_t) + (count - 1) * sizeof(sit_term));
	for(int i = 0	; i < count; i++) {
		memcpy(&con->terms[i], terms[i], sizeof(sit_term));
	}
	qsort(con->terms, count, sizeof(sit_term), qsit_termcmp);	
	con->count = count;
	con->data = NULL;
	return con;
}

void 
sit_query_free(sit_query *query) {
	sit_callback_free(query->callback);
	free(query);
}

pstring *
sit_query_to_s(sit_query *query) {
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
		sit_term *term = &con->terms[i];
		if(term->negated) {
			PC("NOT ");
		}
		P(sit_term_to_s(term));
	}
	PC(")");
	
	return buf;
	
}
