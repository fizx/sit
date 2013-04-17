#include "sit.h"

void
_perc_found_handler(Callback *callback, void *data) {
  long doc_id = *(long*)data;
  Input *input = callback->user_data;
  Output *output = input->output;
  long query_id = callback->id;
  SMALL_OUT("{\"status\": \"ok\", \"message\": \"found\", \"query_id\": ", "%ld, \"doc_id\": %ld}", query_id, doc_id);  
}

void 
_ack_doc(Callback *cb, void *data) {
  Engine *engine = data;
  Input *input = cb->user_data;
  Output *output = input->output;
  if(output) {
    if(engine->error) {
      SMALL_OUT("{\"status\": \"error\", \"message\": \"", "%s\"}", engine->error);
    } else {
      SMALL_OUT("{\"status\": \"ok\", \"message\": \"added\", \"doc_id\": ", "%ld}", engine_last_document_id(engine));
    }
  } else {
    INFO("{\"status\": \"ok\", \"message\": \"added\", \"doc_id\": %ld}", engine_last_document_id(engine));
  }
}

void 
__input_error_found(Callback *cb, void *data) {
  Input *input = cb->user_data;
  Output *output = input->output;
  pstring escaped;
  json_escape(&escaped, data);
  pstring *buf = pstringf("{\"status\": \"error\", \"message\": \"%.*s\"}", escaped.len, escaped.val);
  if(output) {
    output->write(output, buf);
  } else {
    WARN("Error in unidirectional stream: %.*s", escaped.len, escaped.val);
  }
  free(escaped.val);
}

void 
_input_document_found(Callback *cb, void *data) {
  Input *input = cb->user_data;
	Engine *engine = input->engine;
  engine->current_output = input->output;
  engine->after_on_document = input->doc_acker;
  engine_document_found(engine, data);
}

void
_channel_handler(Callback *callback, void *data) {
  Query *query = data;
  Input *input = callback->user_data;
  Output *output = input->output;
  Engine * engine = input->engine;
  if(input->qparser_mode == REGISTERING) {
    query->callback = callback_new(_perc_found_handler, input);
    long query_id = engine_register(engine, query);
    SMALL_OUT("{\"status\": \"ok\", \"message\": \"registered\", \"id\": ", "%ld}", query_id);
    query->callback = NULL; // disassociate from gc.
  } else {
    query->callback = callback_new(_perc_found_handler, input);
    ResultIterator *iter = engine_search(engine, query);
    SMALL_OUT("{\"status\": \"ok\", \"message\": \"querying\", \"id\": ", "%ld}", query->callback->id);
    while(result_iterator_prev(iter) && (query->limit-- != 0)) {
      result_iterator_do_callback(iter);
    }
    SMALL_OUT("{\"status\": \"ok\", \"message\": \"complete\", \"id\": ", "%ld}", query->callback->id);
    result_iterator_free(iter);
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
	input->qparser = query_parser_new(callback_new(_channel_handler, input));
	input->parser = engine->parser->fresh_copy(engine->parser);
	input->parser->on_document = callback_new(_input_document_found, input);
	input->parser->on_error = callback_new(__input_error_found, input);
	input->data = NULL;
  input->doc_acker = callback_new(_ack_doc, input);
	return input;	
}

void
input_free(Input *input) {  
  callback_free(input->qparser->on_query);
  callback_free(input->parser->on_document);
  callback_free(input->parser->on_error);
  query_parser_free(input->qparser);
  parser_free(input->parser);
  callback_free(input->doc_acker);
  free(input);
}


void
input_consume(struct Input *input, pstring *pstr) {
	input->parser->consume(input->parser, pstr);
}

void 
input_end_stream(struct Input *input) {
  input->parser->end_stream(input->parser);
}

