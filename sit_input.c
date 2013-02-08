#include "sit.h"
#include <assert.h>

sit_input *
sit_input_new(struct sit_engine *engine, int term_capacity, long buffer_size) {
 	sit_input *input = malloc(sizeof(sit_input) + (term_capacity - 1) * (sizeof(sit_term)));
	input->parser = engine->parser->fresh_copy(engine->parser);
	input->stream = ring_buffer_new(buffer_size);
	input->term_index = dictCreate(getTermDict(), 0);
	input->term_count = 0;
	input->field = c2pstring("default");
	input->term_capacity = term_capacity;
	input->data = NULL;
	return input;	
}

void
sit_input_consume(struct sit_input *input, pstring *pstr) {
	ring_buffer_append_pstring(input->stream, pstr);
	input->parser->consume(input->parser, pstr);
}

void 
sit_input_term_found(sit_input *input, long off, int len, int field_offset) {
	sit_term *term = &input->terms[input->term_count++];
	term->field = input->field;
	term->text = ring_buffer_get_pstring(input->stream, off, len);
	term->offset = field_offset;
	sit_term_update_hash(term);
	dictAdd(input->term_index, term, term);
}

void 
sit_input_document_found(sit_input *input, long off, int len) {
	assert(off >= 0);
	assert(len > 0);
	sit_engine *engine = input->engine;
  doc_ref dr = { off, len };
	copy_input_ring_buffer_to_engine();
  long doc_id = sit_input_last_document_id(engine);
	sit_engine_percolate(engine, input);
	sit_engine_index(engine, input, doc_id);
  copy_ints_to_sit_engine(engine, input);
	input->term_count = 0;
	dictEmpty(input->term_index);
}

void 
sit_input_int_found(sit_input *input, int value) {
  sit_input_set_int(input, input->field, value);
}

void 
sit_input_field_found(sit_input *input, pstring *name) {
	input->field = name;
}
