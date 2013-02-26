#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include "term.h"

typedef struct Output {
	void *data;
  void (*write)(struct Output *output, pstring *message);
  void (*close)(struct Output *output);
} Output;

typedef enum { REGISTERING, QUERYING } QueryParserMode;

typedef struct QueryIdNode {
  long query_id;
  struct QueryIdNode *next;
} QueryIdNode;

typedef struct Input {
	struct Receiver as_receiver;
  struct Engine *engine;
	Parser      *parser;
  struct query_parser    *qparser;
  
	// fields used to manage the current document
  dict            *term_index;
  int              term_count;
  pstring         *field;
  int              term_capacity;
	dict            *ints;
  
  pstring *stream;
  
  QueryParserMode qparser_mode;
  QueryIdNode    *query_ids;
  
  Output      *output;
  pstring *error;

	void *data;

	struct Term         terms[1];
} Input;

Input *
input_new(struct Engine *engine, int term_capacity, long buffer_size);

void
input_consume(struct Input *input, pstring *pstr);

void 
input_error_found(Receiver *receiver, pstring *name);

void 
input_end_stream(struct Input *input);

void 
input_term_found(Receiver *input, pstring *pstr, int field_offset);

void 
input_document_found(Receiver *input, pstring *pstr);

void 
input_int_found(Receiver *input, int value);

void 
input_field_found(Receiver *input, pstring *name);

#endif