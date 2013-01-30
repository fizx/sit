#ifndef SIT_ENGINE_H_INCLUDED
#define SIT_ENGINE_H_INCLUDED

#include "pstring.h"
#include "dict.h"
#include "lrw_dict.h"
#include "plist.h"
#include "sit_term.h"
#include "sit_parser.h"
#include "sit_query.h"

typedef struct {
  // Data structures & indexes
  dict           *queries;  // Registered for percolation
  sit_parser     *parser;
  ring_buffer    *stream;
  lrw_dict       *term_dictionary;
  plist_pool     *postings;
 
  // fields used to manage the current document
  dict            *term_index;
  int              term_count;
  pstring          *field;
  int              term_capacity;
  ring_buffer     *docs;

  // User-settable
  void *data;
  
  sit_term    terms[1];
} sit_engine;

sit_engine *
sit_engine_new(sit_parser *parser, long size);

void 
sit_engine_term_found(sit_engine *engine, long off, int len, int field_offset);

pstring *
sit_engine_last_document(sit_engine *engine);

pstring *
sit_engine_get_document(sit_engine *engine, long doc_id);

long
sit_engine_last_document_id(sit_engine *engine);

void 
sit_engine_document_found(sit_engine *engine, long off, int len);

void 
sit_engine_field_found(sit_engine *engine, pstring *name);

void
sit_engine_consume(sit_engine *engine, pstring *pstr);

void
sit_engine_percolate(sit_engine *engine);

void
sit_engine_index(sit_engine *engine, long doc_id);

long
sit_engine_register(sit_engine *engine, sit_query *query);

void
sit_engine_unregister(sit_engine *engine, long query_id);

void 
sit_engine_each_query(sit_engine *engine, sit_callback *callback);

#endif

