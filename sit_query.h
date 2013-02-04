#ifndef SIT_QUERY_H_INCLUDED
#define SIT_QUERY_H_INCLUDED

#include "sit_term.h"
#include "sit_callback.h"

typedef struct {
	// for random user state
	void         *data;				

  int           count;	
  sit_term      terms[1];
} conjunction_t;

typedef struct {
  sit_callback *callback;

	// for random user state
	void         *data;				

  int           count;	
  conjunction_t **conjunctions;
} sit_query;

sit_query *
sit_query_new(conjunction_t **conjunctions, int count, sit_callback *callback);

conjunction_t *
conjunction_new(sit_term **terms, int count);

void 
sit_query_free(sit_query *query);

#endif
