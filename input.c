#include "sit.h"

void
_perc_found_handler(Callback *callback, void *data) {
  long doc_id = *(long*)data;
  Input *input = callback->user_data;
  Engine *engine = input->engine;
  pstring *doc = engine_get_document(engine, doc_id);
  long query_id = callback->id;
  pstring *buf = pstring_new(0);
  PV("{\"status\": \"ok\", \"message\": \"found\", \"query_id\": %ld, \"doc_id\": %ld, \"doc\": %.*s}", query_id, doc_id, doc->len, doc->val);
  input->output->write(input->output, buf);
  pstring_free(buf);
}

void
_channel_handler(Callback *callback, void *data) {
  Query *query = data;
  Input *input = callback->user_data;
  Engine * engine = input->engine;
  if(input->qparser_mode == REGISTERING) {
    query->callback = callback_new();
    query->callback->user_data = input;
    query->callback->handler = _perc_found_handler;
    long query_id = engine_register(engine, query);
    QueryIdNode *node = malloc(sizeof(*node));
    node->query_id = query_id;
    node->next = input->query_ids;
    input->query_ids = node;
    pstring *buf = pstring_new(0);
    PV("{\"status\": \"ok\", \"message\": \"registered\", \"id\": %ld}", query_id);
    input->output->write(input->output, buf);
    pstring_free(buf);
  } else {
    query->callback = callback_new();
    query->callback->user_data = input;
    query->callback->handler = _perc_found_handler;
    ResultIterator *iter = engine_search(engine, query);
    pstring *buf = pstring_new(0);
    PV("{\"status\": \"ok\", \"message\": \"querying\", \"id\": %ld}", query->callback->id);
    input->output->write(input->output, buf);
    pstring_free(buf);
    while(result_iterator_prev(iter) && (query->limit-- != 0)) {
      result_iterator_do_callback(iter);
    }
    buf = pstring_new(0);
    PV("{\"status\": \"ok\", \"message\": \"complete\", \"id\": %ld}", query->callback->id);
    input->output->write(input->output, buf);
    pstring_free(buf);
  } 
}

Input *
input_new(struct Engine *engine, int term_capacity, long buffer_size) {
	assert(engine);
	assert(engine->parser);
 	Input *input = calloc(1, sizeof(Input) + (term_capacity - 1) * (sizeof(Term)));
	input->engine = engine;
  input->qparser_mode = REGISTERING;
	input->qparser = query_parser_new();
	input->qparser->cb = callback_new();
	input->qparser->cb->handler = _channel_handler;
  input->qparser->cb->user_data = input;
	input->parser = engine->parser->fresh_copy(engine->parser);
	input->parser->receiver = &input->as_receiver;
	input->as_receiver.term_found     = input_term_found;
	input->as_receiver.document_found = input_document_found;
	input->as_receiver.field_found    = input_field_found;
	input->as_receiver.int_found      = input_int_found;
	input->as_receiver.error_found     = input_error_found;
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
input_consume(struct Input *input, pstring *pstr) {
  padd(input->stream, pstr);
  assert(input->parser->receiver == &input->as_receiver);
	input->parser->consume(input->parser, pstr);
}

void 
input_end_stream(struct Input *input) {
  input->parser->end_stream(input->parser);
}

void 
_ack_doc(Callback *cb, void *data) {
  Engine *engine = data;
  Input *input = cb->user_data;
  Output *output = input->output;
  pstring *buf = pstring_new(0);
  PC("{\"status\": \"ok\", \"message\": \"added\", \"doc_id\": ");
  PV("%ld", engine_last_document_id(engine));
  PC("\"}");
  output->write(output, buf);
  pstring_free(buf);    
}

void 
input_error_found(Receiver *receiver, pstring *message) {
	Input *input = (Input *)receiver;
  Output *output = input->output;
  pstring *buf = pstring_new(0);
  PC("{\"status\": \"error\", \"message\": \"");
  P(message);
  PC("\"}");
  output->write(output, buf);
  pstring_free(buf);
}

void 
input_field_found(Receiver *receiver, pstring *name) {
	Input *input = (Input *)receiver;
	input->field = name;
}

void 
input_term_found(Receiver *receiver, pstring *pstr, int field_offset) {
	Input *input = (Input *)receiver;
	Term *term = &input->terms[input->term_count++];
	term->field = input->field;
  term->text = pcpy(pstr);
	term->offset = field_offset;
	term_update_hash(term);
	dictAdd(input->term_index, term, term);
}

void 
input_int_found(Receiver *receiver, int value) {
	Input *input = (Input *)receiver;
	dictEntry *entry = dictReplaceRaw(input->ints, input->field);
	dictSetSignedIntegerVal(entry, value);
}

void 
input_document_found(Receiver *receiver, long off, int len) {
	Input *input = (Input *)receiver;
	assert(off >= 0);
	assert(len > 0);
	Engine *engine = input->engine;
	ring_buffer_append(engine->stream, (void*)input->stream->val, len);
	engine->current_input = input;
  Callback *old = engine->on_document_found;
  Callback cb = {
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
    engine_int_found(&engine->as_receiver, dictGetSignedIntegerVal(entry));
  }
  
  engine->on_document_found = &cb;
	engine_document_found(&engine->as_receiver, engine->stream->written - len, len);
  engine->on_document_found = old;
  dictReleaseIterator(iter);
	input->stream = pstring_new(0);
  input->term_count = 0;
	dictEmpty(input->term_index);
	engine->current_input = NULL;
}