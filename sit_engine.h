#ifndef SIT_ENGINE_H_INCLUDED
#define SIT_ENGINE_H_INCLUDED

#include "pstring.h"
#include "dict.h"
#include "lrw_dict.h"
#include "plist.h"
#include "sit_term.h"
#include "sit_parser.h"

typedef struct {
  // Data structures & indexes
  dict           *queries;  // Registered for percolation
	sit_parser     *parser;
  void           *stream;
	void          (*stream_append)(void *stream, pstring *buffer);
	pstring      *(*stream_get)(void *stream, long off, int len);
  lrw_dict       *term_dictionary;
  plist_pool     *postings;
 
	// fields used to manage the current document
	dict       *term_index;
  int         term_count;
	pstring	   *field;
	int 				term_capacity;
	long				off;
	int 				len;

	// User-settable
	void *data;
	
  sit_term    terms[1];
} sit_engine;

sit_engine *
sit_engine_new(sit_parser *parser, long size);

void 
sit_engine_term_found(void *engine, long off, int len, int field_offset);

void 
sit_engine_document_found(void *engine, long off, int len);

void 
sit_engine_field_found(void *engine, pstring *name);

void
sit_engine_consume(void *engine, pstring *pstr);

#endif

