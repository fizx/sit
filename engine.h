#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

#include "term.h"
#include "query.h"
#include "input.h"
#include "receiver.h"
#include "plist.h"

typedef struct Engine {
	struct Receiver as_receiver;
  // Data structures & indexes
  dict                  *queries;  // Registered for percolation
  Parser                *parser;
  struct ring_buffer    *stream;
  struct LRWDict        *term_dictionary;
  struct PlistPool      *postings;

  dict           *ints;
  long            ints_capacity;
 
  // fields used to manage the current document
  dict            *term_index;
  int              term_count;
  pstring         *field;
  int              term_capacity;
  struct ring_buffer     *docs;
	struct sit_input       *current_input;

  // User-settable
  void *data;
  
  struct Callback *on_document_found;
  
  long                query_id;
  sit_term    terms[1];
} Engine;

typedef struct doc_ref {
	long off;
	int len;
} doc_ref;

typedef struct {
  int count;
  long doc_id;
  bool initialized;
  struct Cursor **cursors;
  long *state;
  int *negateds;
} sub_iterator;

typedef struct {
  Engine *engine;
  long doc_id;
  Query *query;
  bool initialized;
  int count;
  sub_iterator **subs;
  dict *cursors;
} sit_result_iterator;


Engine *
sit_engine_new(Parser *parser, long size);

sit_result_iterator *
sit_engine_search(Engine *engine, Query *query);

void 
sit_engine_term_found(Receiver *receiver, pstring *pstr, int field_offset);

void 
sit_engine_document_found(Receiver *receiver, long off, int len);

void 
sit_engine_field_found(Receiver *receiver, pstring *name);

void 
sit_engine_int_found(Receiver *receiver, int value);

int *
sit_engine_get_int(Engine *engine, long doc_id, pstring *field);

void
sit_engine_set_int(Engine *engine, long doc_id, pstring *field, int value);

void
sit_engine_incr(Engine *engine, long doc_id, pstring *field, int value);

pstring *
sit_engine_last_document(Engine *engine);

pstring *
sit_engine_get_document(Engine *engine, long doc_id);

long
sit_engine_last_document_id(Engine *engine);

void
sit_engine_consume(Engine *engine, pstring *pstr);

void
sit_engine_percolate(Engine *engine);

void
sit_engine_index(Engine *engine, long doc_id);

long
sit_engine_register(Engine *engine, Query *query);

void 
sit_engine_each_node(Engine *engine, struct Callback *cb);

bool
sit_engine_unregister(Engine *engine, long query_id);

void 
sit_engine_each_query(Engine *engine, struct Callback *callback);

bool
sit_result_iterator_prev(sit_result_iterator *iter);

void
sit_result_iterator_do_callback(sit_result_iterator *iter);

pstring *
sit_result_iterator_document(sit_result_iterator *iter);

long
sit_result_iterator_document_id(sit_result_iterator *iter);

#endif
