#include "sit_query.h"
#include <stdlib.h>
#include <stdio.h>

int
qsit_termcmp(const void *a, const void *b) {
	sit_term **ta = (sit_term **)a;
	sit_term **tb = (sit_term **)b;
	return sit_termcmp(*ta, *tb);
}

sit_query *
sit_query_new(sit_term **terms, int term_count, sit_callback *callback) {
	sit_query *query = malloc(sizeof(sit_query));
	sit_term **dup  = malloc(sizeof(sit_term*) * term_count);
	for(int i = 0; i < term_count; i++) {
		dup[i] = terms[i];
	}
	qsort(dup, term_count, sizeof(sit_term *), qsit_termcmp);	
	query->terms = dup;
	query->term_count = term_count;
	query->callback = callback;
	query->data = NULL;
	return query;
}

void 
sit_query_free(sit_query *query) {
	sit_callback_free(query->callback);
	for(int i = 0; i < query->term_count; i++) {
		sit_term_free(query->terms[i]);
	}
	free(query->terms);
	free(query);
}
