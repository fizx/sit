#include "sit.h"
#include <assert.h>
#include <stdbool.h>
#include "ruby.h"
#include "util_ruby.h"

void
_perc_found_handler(sit_callback *callback, void *data) {
  long doc_id = *(long*)data;
  sit_input *input = callback->user_data;
  sit_engine *engine = input->engine;
  pstring *doc = sit_engine_get_document(engine, doc_id);
  long query_id = callback->id;
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
    query->callback = sit_callback_new();
    query->callback->user_data = input;
    query->callback->handler = _perc_found_handler;
    sit_result_iterator *iter = sit_engine_search(engine, query);
    pstring *buf = pstring_new(0);
    PV("{\"status\": \"ok\", \"message\": \"querying\", \"id\": %ld}", query->callback->id);
    input->output->write(input->output, buf);
    pstring_free(buf);
    while(sit_result_iterator_prev(iter) && (query->limit-- != 0)) {
      sit_result_iterator_do_callback(iter);
    }
    buf = pstring_new(0);
    PV("{\"status\": \"ok\", \"message\": \"complete\", \"id\": %ld}", query->callback->id);
    input->output->write(input->output, buf);
    pstring_free(buf);
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
	input->as_receiver.error_found     = sit_input_error_found;
	input->stream = pstring_new(0);
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
  padd(input->stream, pstr);
  assert(input->parser->receiver == &input->as_receiver);
	input->parser->consume(input->parser, pstr);
}

void 
sit_input_term_found(sit_receiver *receiver, pstring *pstr, int field_offset) {
	sit_input *input = (sit_input *)receiver;
	sit_term *term = &input->terms[input->term_count++];
	term->field = input->field;
  term->text = pcpy(pstr);
	term->offset = field_offset;
	sit_term_update_hash(term);
	dictAdd(input->term_index, term, term);
}

void 
sit_input_end_stream(struct sit_input *input) {
  input->parser->end_stream(input->parser);
}

void 
sit_input_error_found(sit_receiver *receiver, pstring *message) {
	sit_input *input = (sit_input *)receiver;
  sit_output *output = input->output;
  pstring *buf = pstring_new(0);
  PC("{\"status\": \"error\", \"message\": \"");
  P(message);
  PC("\"}");
  output->write(output, buf);
  pstring_free(buf);
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
_ack_doc(sit_callback *cb, void *data) {
  sit_engine *engine = data;
  sit_input *input = cb->user_data;
  sit_output *output = input->output;
  pstring *buf = pstring_new(0);
  PC("{\"status\": \"ok\", \"message\": \"added\", \"doc_id\": ");
  PV("%ld", sit_engine_last_document_id(engine));
  PC("\"}");
  output->write(output, buf);
  pstring_free(buf);    
}

void 
sit_input_document_found(sit_receiver *receiver, long off, int len) {
	sit_input *input = (sit_input *)receiver;
	assert(off >= 0);
	assert(len > 0);
	sit_engine *engine = input->engine;
	ring_buffer_append(engine->stream, (void*)input->stream->val, len);
	engine->current_input = input;
  sit_callback *old = engine->on_document_found;
  sit_callback cb = {
    _ack_doc,
    input,
    NULL,
    -1,
    NULL
  };
  dictIterator *iter = dictGetIterator(input->ints);
  dictEntry *entry;

  while ((entry = dictNext(iter))) {
    engine->field = dictGetKey(entry);
    sit_engine_int_found(&engine->as_receiver, dictGetSignedIntegerVal(entry));
  }
  
  engine->on_document_found = &cb;
	sit_engine_document_found(&engine->as_receiver, engine->stream->written - len, len);
  engine->on_document_found = old;
  dictReleaseIterator(iter);
	input->stream = pstring_new(0);
  input->term_count = 0;
	dictEmpty(input->term_index);
	engine->current_input = NULL;
}