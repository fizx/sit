#include "sit.h"
#include <assert.h>
#include <stdbool.h>
#include "ruby.h"
#include "util_ruby.h"

void
_perc_found_handler(sit_callback *callback, void *data) {
  pstring *doc = data;
  sit_input *input = callback->user_data;
  long query_id = callback->id;
  long doc_id = sit_engine_last_document_id(input->engine);
  pstring *buf = pstring_new(0);
  PV("{\"status\": \"ok\", \"message\": \"found\", \"query_id\": %ld, \"doc_id\": %ld, \"doc\": %.*s}", query_id, doc_id, doc->len, doc->val);
  input->output->write(input->output, buf);
  pstring_free(buf);
}


void
_channel_handler(sit_callback *callback, void *data) {
  sit_query *query = data;
  sit_input *input = callback->user_data;
  sit_engine * engine = input->engine;
  if(input->qparser_mode == REGISTERING) {
    query->callback = sit_callback_new();
    query->callback->user_data = input;
    query->callback->handler = _perc_found_handler;
    long query_id = sit_engine_register(engine, query);
    query_id_node *node = malloc(sizeof(*node));
    node->query_id = query_id;
    node->next = input->query_ids;
    input->query_ids = node;
    pstring *buf = pstring_new(0);
    PV("{\"status\": \"ok\", \"message\": \"registered\", \"id\": %ld}", query_id);
    input->output->write(input->output, buf);
    pstring_free(buf);
  } else {
    
  } 
}

sit_input *
sit_input_new(struct sit_engine *engine, int term_capacity, long buffer_size) {
	assert(engine);
	assert(engine->parser);
 	sit_input *input = calloc(1, sizeof(sit_input) + (term_capacity - 1) * (sizeof(sit_term)));
	input->engine = engine;
  input->qparser_mode = REGISTERING;
	input->qparser = query_parser_new();
	input->qparser->cb = sit_callback_new();
	input->qparser->cb->handler = _channel_handler;
  input->qparser->cb->user_data = input;
	input->parser = engine->parser->fresh_copy(engine->parser);
	input->parser->receiver = &input->as_receiver;
	input->as_receiver.term_found     = sit_input_term_found;
	input->as_receiver.document_found = sit_input_document_found;
	input->as_receiver.field_found    = sit_input_field_found;
	input->as_receiver.int_found      = sit_input_int_found;
	input->stream = ring_buffer_new(buffer_size);
	input->term_index = dictCreate(getTermDict(), 0);
	input->ints = dictCreate(getPstrDict(), 0);
	input->term_count = 0;
	input->field = c2pstring("default");
	input->term_capacity = term_capacity;
	input->data = NULL;
	input->error = NULL;
	return input;	
}

void
sit_input_consume(struct sit_input *input, pstring *pstr) {
	ring_buffer_append_pstring(input->stream, pstr);
  assert(input->parser->receiver == &input->as_receiver);
	input->parser->consume(input->parser, pstr);
}

void 
sit_input_term_found(sit_receiver *receiver, long off, int len, int field_offset) {
	sit_input *input = (sit_input *)receiver;
	sit_term *term = &input->terms[input->term_count++];
	term->field = input->field;
	term->text = ring_buffer_get_pstring(input->stream, off, len);
	term->offset = field_offset;
	sit_term_update_hash(term);
	dictAdd(input->term_index, term, term);
}

void 
sit_input_end_stream(struct sit_input *input) {
  input->parser->end_stream(input->parser);
}

void 
sit_input_field_found(sit_receiver *receiver, pstring *name) {
	sit_input *input = (sit_input *)receiver;
	input->field = name;
}

void 
sit_input_int_found(sit_receiver *receiver, int value) {
	sit_input *input = (sit_input *)receiver;
	dictEntry *entry = dictReplaceRaw(input->ints, input->field);
	dictSetSignedIntegerVal(entry, value);
}

void 
sit_input_document_found(sit_receiver *receiver, long off, int len) {
	sit_input *input = (sit_input *)receiver;
	assert(off >= 0);
	assert(len > 0);
	sit_engine *engine = input->engine;
	ring_buffer_append(engine->stream, input->stream->buffer + off, len);
	engine->current_input = input;
	sit_engine_document_found(&engine->as_receiver, engine->stream->written - len, len);
  long doc_id = sit_engine_last_document_id(engine);
  dictIterator *iter = dictGetIterator(input->ints);
  dictEntry *entry;
  while ((entry = dictNext(iter))) {
    sit_engine_set_int(engine, doc_id, dictGetKey(entry), dictGetSignedIntegerVal(entry));
  }
  dictReleaseIterator(iter);
  input->term_count = 0;
	dictEmpty(input->term_index);
	engine->current_input = NULL;
}