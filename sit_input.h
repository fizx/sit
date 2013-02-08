#ifndef SIT_INPUT_H_INCLUDED
#define SIT_INPUT_H_INCLUDED

#include "sit.h"

struct sit_engine;

typedef struct sit_input {
  struct sit_engine *engine;
	sit_parser      *parser;
  
	// fields used to manage the current document
  dict            *term_index;
  int              term_count;
  pstring         *field;
  int              term_capacity;
  
  ring_buffer     *stream;

	void *data;

	sit_term         terms[1];
} sit_input;

sit_input *
sit_input_new(struct sit_engine *engine, int term_capacity, long buffer_size);

void
sit_input_consume(struct sit_input *input, pstring *pstr);

#endif