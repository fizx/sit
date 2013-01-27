#include "sit_query.h"
#include <stdlib.h>

sit_query *
sit_query_new(sit_term **terms, int term_count, sit_callback *callback) {
	sit_query *query = malloc(sizeof(sit_query));
	query->terms = terms;
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