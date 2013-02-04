#include "sit_query.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int
qsit_termcmp(const void *a, const void *b) {
	sit_term **ta = (sit_term **)a;
	sit_term **tb = (sit_term **)b;
	return sit_termcmp(*ta, *tb);
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
	qsort(con->terms, count, sizeof(sit_term), sit_termcmp);	
	con->count = count;
	con->data = NULL;
	return con;
}

void 
sit_query_free(sit_query *query) {
	sit_callback_free(query->callback);
	free(query);
}
