#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

#include "term.h"
#include "query.h"
#include "input.h"
#include "plist.h"

typedef struct QueryNode {
	dict               *children;
	Term							 *term;
	struct QueryNode   *parent;
	Callback 				   *callback;
} QueryNode;

void 
query_node_free(QueryNode *node);

typedef struct Engine {
  // Data structures & indexes
  dict                  *queries;  // Registered for percolation
  Parser                *parser;
  struct RingBuffer     *stream;
  struct LRWDict        *term_dictionary;
  struct PlistPool      *postings;
  
  pstring               *data_dir;
  FILE                  *active_journal;
  long                  last_offset;
  
  pstring               *auth;

  dict                  *ints;
  long                   ints_capacity;
  struct RingBuffer     *docs;
  dict                  *doc_set;
  
  long                   min_doc_id;
  
	Callback 				      *catchall_callbacks;
  
  char *error;
 
  // User-settable
  void *data;
  
  struct Callback    *after_on_document;
  Output             *current_output;
  
  long                query_id;
} Engine;

typedef struct DocRef {
  pstring *tmp;
	long off;
	int len;
	unsigned int hash_code;
} DocRef;

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
} ResultIterator;

Engine *
engine_new(Parser *parser, pstring *data_dir, long size, bool dedupe, pstring *auth);

void
engine_free(Engine *engine);

void
engine_replay_journal(Engine *engine);

void
engine_append_journal(Engine *engine, pstring *str);

void
engine_reopen_journal(Engine *engine);

void
engine_fsync_journal(Engine *engine);


ResultIterator *
engine_search(Engine *engine, Query *query);

int *
engine_get_int(Engine *engine, long doc_id, pstring *field);

void
engine_set_int(Engine *engine, long doc_id, pstring *field, int value);

void
engine_incr(Engine *engine, long doc_id, pstring *field, int value);

long
engine_min_document_id(Engine *engine);

pstring *
engine_last_document(Engine *engine);

pstring *
engine_get_document(Engine *engine, long doc_id);

void 
engine_document_found(Engine *engine, DocBuf *buffer);

long
engine_last_document_id(Engine *engine);

void
engine_percolate(Engine *engine, DocBuf *buffer, long doc_id);

void
engine_index(Engine *engine, DocBuf *buffer, long doc_id);

void
result_iterator_free(ResultIterator *iter);

long
engine_register(Engine *engine, Query *query);

void 
engine_each_node(Engine *engine, struct Callback *cb);

bool
engine_unregister(Engine *engine, long query_id);

void 
engine_each_query(Engine *engine, struct Callback *callback);

bool
result_iterator_prev(ResultIterator *iter);

void
result_iterator_do_callback(ResultIterator *iter);

pstring *
result_iterator_document(ResultIterator *iter);

long
result_iterator_document_id(ResultIterator *iter);

#endif
