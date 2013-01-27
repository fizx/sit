#ifndef SIT_QUERY_H_INCLUDED
#define SIT_QUERY_H_INCLUDED

#include "sit_term.h"
#include "sit_callback.h"

typedef struct {
  sit_term    **terms;
  int           term_count;
  sit_callback *callback;

	// for random user state
	void         *data;				
} sit_query;

sit_query *
sit_query_new(sit_term **terms, int term_count, sit_callback *callback);

void 
sit_query_free(sit_query *query);

#endif