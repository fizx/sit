#ifndef SIT_ENGINE_H_INCLUDED
#define SIT_ENGINE_H_INCLUDED

#include "pstring.h"
#include "dict.h"
#include "lrw_dict.h"
#include "plist.h"
#include "cursor.h"
#include "input.h"
#include "parser.h"
#include "query.h"

struct plist_pool;

typedef struct sit_engine {
	struct sit_receiver as_receiver;
  // Data structures & indexes
  dict           *queries;  // Registered for percolation
  sit_parser     *parser;
  struct ring_buffer    *stream;
  struct lrw_dict       *term_dictionary;
  struct plist_pool     *postings;

  dict           *ints;
  long            ints_capacity;
 
  // fields used to manage the current document
  dict            *term_index;
  int              term_count;
  pstring         *field;
  int              term_capacity;
  struct ring_buffer     *docs;
	sit_input       *current_input;

  // User-settable
  void *data;
  
  Callback *on_document_found;
  
  long                query_id;
  sit_term    terms[1];
} sit_engine;

typedef struct doc_ref {
	long off;
	int len;
} doc_ref;

typedef struct {
  int count;
  long doc_id;
  bool initialized;
  Cursor **cursors;
  long *state;
  int *negateds;
} sub_iterator;

typedef struct {
  sit_engine *engine;
  long doc_id;
  sit_query *query;
  bool initialized;
  int count;
  sub_iterator **subs;
  dict *cursors;
} sit_result_iterator;


sit_engine *
sit_engine_new(sit_parser *parser, long size);

sit_result_iterator *
sit_engine_search(sit_engine *engine, sit_query *query);

void 
sit_engine_term_found(sit_receiver *receiver, pstring *pstr, int field_offset);

void 
sit_engine_document_found(sit_receiver *receiver, long off, int len);

void 
sit_engine_field_found(sit_receiver *receiver, pstring *name);

void 
sit_engine_int_found(sit_receiver *receiver, int value);

int *
sit_engine_get_int(sit_engine *engine, long doc_id, pstring *field);

void
sit_engine_set_int(sit_engine *engine, long doc_id, pstring *field, int value);

void
sit_engine_incr(sit_engine *engine, long doc_id, pstring *field, int value);

pstring *
sit_engine_last_document(sit_engine *engine);

pstring *
sit_engine_get_document(sit_engine *engine, long doc_id);

long
sit_engine_last_document_id(sit_engine *engine);

void
sit_engine_consume(sit_engine *engine, pstring *pstr);

void
sit_engine_percolate(sit_engine *engine);

void
sit_engine_index(sit_engine *engine, long doc_id);

long
sit_engine_register(sit_engine *engine, sit_query *query);

void 
sit_engine_each_node(sit_engine *engine, Callback *cb);

bool
sit_engine_unregister(sit_engine *engine, long query_id);

void 
sit_engine_each_query(sit_engine *engine, Callback *callback);

bool
sit_result_iterator_prev(sit_result_iterator *iter);

void
sit_result_iterator_do_callback(sit_result_iterator *iter);

pstring *
sit_result_iterator_document(sit_result_iterator *iter);

long
sit_result_iterator_document_id(sit_result_iterator *iter);

#endif
