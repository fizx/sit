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
__input_error_found(Callback *cb, void *data) {
  Input *input = cb->user_data;
  pstring *message = data;
  Output *output = input->output;
  pstring *buf = pstring_new(0);
  PC("{\"status\": \"error\", \"message\": \"");
  P(message);
  PC("\"}");
  output->write(output, buf);
  pstring_free(buf);
}

void 
_input_document_found(Callback *cb, void *data) {
  Input *input = cb->user_data;
	Engine *engine = input->engine;
  engine->current_output = input->output;
  engine_document_found(engine, data);
}

void
_channel_handler(Callback *callback, void *data) {
  Query *query = data;
  Input *input = callback->user_data;
  Engine * engine = input->engine;
  if(input->qparser_mode == REGISTERING) {
    query->callback = callback_new(_perc_found_handler, input);
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
    query->callback = callback_new(_perc_found_handler, input);
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
input_new(struct Engine *engine, long buffer_size) {
  (void) buffer_size;
	assert(engine);
	assert(engine->parser);
 	Input *input = malloc(sizeof(Input));
	input->engine = engine;
  input->qparser_mode = REGISTERING;
	input->qparser = query_parser_new();
	input->qparser->on_query = callback_new(_channel_handler, input);
	input->parser = engine->parser->fresh_copy(engine->parser);
	input->parser->on_document = callback_new(_input_document_found, input);
	input->parser->on_error = callback_new(__input_error_found, input);
	input->data = NULL;
	return input;	
}

void
input_consume(struct Input *input, pstring *pstr) {
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

