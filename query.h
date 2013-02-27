#ifndef SIT_QUERY_H_INCLUDED
#define SIT_QUERY_H_INCLUDED

#include "term.h"
#include "callback.h"

typedef struct {
	// for random user state
	void         *data;				

  int           count;	
  sit_term      terms[1];
} conjunction_t;

typedef struct {
  Callback *callback;

	// for random user state
	void         *data;				
  int           limit;
  int           count;	
  conjunction_t **conjunctions;
} sit_query;

int
qsit_termcmp(const void *a, const void *b);

sit_query *
sit_query_new(conjunction_t **conjunctions, int count, Callback *callback);

conjunction_t *
conjunction_new(sit_term **terms, int count);

void 
sit_query_free(sit_query *query);

pstring *
sit_query_to_s(sit_query *query);

pstring *
conjunction_to_s(conjunction_t *cj);


#endif
