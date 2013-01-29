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
sit_query_new(sit_term **terms, int term_count, sit_callback *callback) {
	sit_query *query = malloc(sizeof(sit_query) + (term_count - 1) * sizeof(sit_term));
	for(int i = 0	; i < term_count; i++) {
		memcpy(&query->terms[i], terms[i], sizeof(sit_term));
	}
	qsort(query->terms, term_count, sizeof(sit_term), sit_termcmp);	
	query->term_count = term_count;
	query->callback = callback;
	query->data = NULL;
	return query;
}

void 
sit_query_free(sit_query *query) {
	sit_callback_free(query->callback);
	free(query);
}
