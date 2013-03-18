#include "sit.h"

#define COMMAND_LIMIT 16

void
_input_error_found(struct ProtocolHandler *handler, pstring *message) {
  Input *input = handler->data;
  Output *output = input->output;
  pstring escaped;
  json_escape(&escaped, message);
  WRITE_OUT("{\"status\": \"error\", \"message\": \"", "%.*s\"}", escaped.len, escaped.val);
  free(escaped.val);
}

int
extract_string(pstring *target, pstring *haystack, int off) {
  for (int i = off; i < haystack->len; i++) {
    if (haystack->val[i] == ' ' || haystack->val[i] == '\r') {
      target->val = haystack->val + off;
      target->len = i - off;
      return i + 1;
    }
  }
  target->val = haystack->val + off;
  target->len = haystack->len - off;
  return haystack->len;
}

void 
_parse_command(ProtocolParser *parser, pstring *pstr) {
  pstring cmd;
  pstring more;
  int off = 0;
  off = extract_string(&cmd, pstr, off);
  more.len = pstr->len - off;
  more.val = pstr->val + off;
  if(!cmd.len) return; // empty line
  parser->handler->command_found(parser->handler, &cmd, &more);
}

void 
_line_consume(ProtocolParser *parser, pstring *pstr) {  
  ProtocolHandler *handler = parser->handler;
  if(parser->state == FORCE_DATA) {
    handler->data_found(handler, pstr);
    return;
  }
  
  char *buf;
  pstring tmp = {
    pstr->val,
    pstr->len
  };
  while ((buf = memchr(tmp.val, '\n', tmp.len))) {
    tmp.len = buf - tmp.val;
    if(tmp.val[0] == '{' || parser->state != COMPLETE) {
      handler->data_found(handler, &tmp);
      if(parser->state != FORCE_DATA) {
        handler->data_complete(handler);
        parser->state = COMPLETE;
      }
    } else {
      _parse_command(parser, &tmp);
    }
    tmp.val += tmp.len + 1;
    tmp.len = pstr->len - (tmp.val - pstr->val);
  }
  if(tmp.len){
    if(parser->state == COMPLETE && tmp.val[0] != '{') {
      handler->error_found(handler, c2pstring("Command was too long"));
    } else {
      handler->data_found(handler, &tmp);
      parser->state = PARTIAL;
    }
  }
}

void
_line_end_stream(ProtocolParser *parser) {
  (void) parser;
}

void
_dump_handler(struct Callback *self, void *data) {
  Query *query = data;
  Output *output = self->user_data;
  output->write(output, query_to_s(query));
}

void
_input_command_found(struct ProtocolHandler *handler, pstring *command, pstring *more) {
  DEBUG("found cmd:  %.*s", command->len, command->val);
  Input *input = handler->data;
  Output *output = input->output;
  Engine *engine = input->engine;
  
  if(!cpstrcmp("register", command)) {
    input->qparser_mode = REGISTERING;
    INFO("registering: %.*s", more->len, more->val);
    query_parser_consume(input->qparser, more);
    query_parser_reset(input->qparser);
  } else if(!cpstrcmp("query", command)) {
    input->qparser_mode = QUERYING;
    query_parser_consume(input->qparser, more);
    query_parser_reset(input->qparser);
  } else if(!cpstrcmp("tail", command)) {
    Task *task = tail_task_new(engine, more, 1.);
  } else if(!cpstrcmp("tasks", command)) {
    WRITE_OUT("{\"status\": \"ok\", \"message\": \"begin\"}", "");
    dictIterator * iterator = dictGetIterator(engine->tasks);
  	dictEntry *next;
  	while((next = dictNext(iterator))) {
      Task *task = dictGetKey(next);
      pstring *json = task_to_json(task);
      WRITE_OUT("{\"status\": \"ok\", \"id\": \"", "%d, details: %.*s}", task->id, json->len, json->val);
      pstring_free(json);
  	}
    dictReleaseIterator(iterator);
    WRITE_OUT("{\"status\": \"ok\", \"message\": \"complete\"}", "");
    Task *task = tail_task_new(engine, more, 1.);
  } else if(!cpstrcmp("stream", command)) {
    Parser *parser = engine_new_stream_parser(engine, more);
    if(parser) {
      parser->on_document = input->parser->on_document;
      parser->on_error = input->parser->on_error;
      parser_free(input->parser);
      input->parser = parser;
      handler->parser->state = FORCE_DATA;      
      WRITE_OUT("{\"status\": \"ok\", \"message\": \"streaming\"}", "");
    } else {
      WRITE_OUT("{\"status\": \"error\", \"message\": \"no stream parser\"}", "");
    }
  } else if(!cpstrcmp("unregister", command)) {
    long query_id = strtol(more->val, NULL, 10);
    bool success = engine_unregister(input->engine, query_id);
    if(success) {
      WRITE_OUT("{\"status\": \"ok\", \"message\": \"unregistered\", \"query_id\": ", "%ld}", query_id);
    } else {
      WRITE_OUT("{\"status\": \"error\", \"message\": \"not found\", \"query_id\": ", "%ld}", query_id);
    }
  } else if(!cpstrcmp("get", command)) {
    long doc_id = strtol(more->val, NULL, 10);
    pstring *doc = engine_get_document(input->engine, doc_id);
    if(doc) {
      WRITE_OUT("{\"status\": \"ok\", \"message\": \"get success\", \"doc\": %.*s}", doc->len, doc->val);
    } else {
      WRITE_OUT("{\"status\": \"error\", \"message\": \"not found\", \"doc_id\": ", "%ld}", doc_id);
    }
  } else if(!cpstrcmp("close", command)) {
    input->output->close(input->output);
  } else if(TEST_MODE && !cpstrcmp("dump", command)) {
    Callback *cb = callback_new(_dump_handler, output);
    engine_each_query(input->engine,  cb);
#ifdef HAVE_EV_H
  } else if(TEST_MODE && !cpstrcmp("stop", command)) {
    INFO("stopping now!\n");
    input->output->close(input->output);
    ev_unloop(ev_default_loop(0), EVUNLOOP_ALL);
    engine_free(input->engine);
    INFO("stopped\n");
#endif
  } else {
    pstring *buf = pstringf("Unknown command: %.*s", command->len, command->val);
    _input_error_found(handler, buf);
    pstring_free(buf);
  }
}

void
_input_data_found(struct ProtocolHandler *handler, pstring *data) {
  DEBUG("found data: %.*s\n", data->len, data->val);
  Input *input = handler->data;
  input_consume(input, data);
}

void
_input_data_complete(struct ProtocolHandler *handler) {
  (void) handler;
}

ProtocolParser *
line_input_protocol_new(Input *input) {
  ProtocolParser * parser = line_protocol_new();
  ProtocolHandler *handler = parser->handler;
  parser->data = NULL;
  handler->data = input;
  handler->command_found = _input_command_found;
  handler->data_found    = _input_data_found   ;
  handler->data_complete = _input_data_complete;
  handler->error_found   = _input_error_found  ;
  return parser;
}

void
line_input_protocol_free(ProtocolParser *parser) {
  free(parser->handler);
  free(parser);
}

ProtocolParser *
line_protocol_new() {
  ProtocolParser *parser = calloc(1, sizeof(ProtocolParser));
  ProtocolHandler *handler = calloc(1, sizeof(ProtocolHandler));
  parser->handler = handler;
  handler->parser = parser;
  parser->consume = _line_consume;
  parser->end_stream = _line_end_stream;
  handler->command_found = NULL;
  handler->data_found = NULL;
  handler->data_complete = NULL;
  handler->error_found = NULL;
  parser->data = NULL;
  handler->data = NULL;
  return parser;
}
