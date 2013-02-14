#ifndef SIT_INPUT_H_INCLUDED
#define SIT_INPUT_H_INCLUDED

#include "sit.h"
#include "sit_term.h"

typedef struct sit_output {
	void *data;
  void (*write)(struct sit_output *output, pstring *message);
  void (*close)(struct sit_output *output);
} sit_output;

typedef enum { REGISTERING, QUERYING } query_parser_mode;

typedef struct query_id_node {
  long query_id;
  struct query_id_node *next;
} query_id_node;

typedef struct sit_input {
	struct sit_receiver as_receiver;
  struct sit_engine *engine;
	sit_parser      *parser;
  struct query_parser    *qparser;
  
	// fields used to manage the current document
  dict            *term_index;
  int              term_count;
  pstring         *field;
  int              term_capacity;
	dict            *ints;
  
  struct ring_buffer     *stream;
  
  query_parser_mode qparser_mode;
  query_id_node    *query_ids;
  
  sit_output      *output;
  pstring *error;

	void *data;

	struct sit_term         terms[1];
} sit_input;

sit_input *
sit_input_new(struct sit_engine *engine, int term_capacity, long buffer_size);

void
sit_input_consume(struct sit_input *input, pstring *pstr);

void 
sit_input_error_found(sit_receiver *receiver, pstring *name);

void 
sit_input_end_stream(struct sit_input *input);

void 
sit_input_term_found(sit_receiver *input, pstring *pstr, int field_offset);

void 
sit_input_document_found(sit_receiver *input, long off, int len);

void 
sit_input_int_found(sit_receiver *input, int value);

void 
sit_input_field_found(sit_receiver *input, pstring *name);

#endif