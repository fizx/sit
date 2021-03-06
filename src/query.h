#ifndef SIT_QUERY_H_INCLUDED
#define SIT_QUERY_H_INCLUDED

#include "term.h"
#include "callback.h"

typedef struct {
	// for random user state
	void         *data;				

  int           count;	
  Term      terms[1];
} conjunction_t;

typedef struct {
  Callback *callback;

	// for random user state
	void         *data;				
  int           limit;
  int           count;	
  conjunction_t **conjunctions;
} Query;

int
qtermcmp(const void *a, const void *b);

Query *
query_new(conjunction_t **conjunctions, int count, Callback *callback);

conjunction_t *
conjunction_new(Term **terms, int count);

void 
query_free(Query *query);

pstring *
query_to_s(Query *query);

pstring *
conjunction_to_s(conjunction_t *cj);


#endif
