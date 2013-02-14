#include "sit_protocol.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "util_ruby.h"
#ifdef HAVE_EV_H
#include <ev.h>
#endif

#define COMMAND_LIMIT 16

void
_input_error_found(struct sit_protocol_handler *handler, pstring *message) {
  sit_input *input = handler->data;
  sit_output *output = input->output;
  pstring *buf = pstring_new(0);
  PC("{\"status\": \"error\", \"message\": \"");
  P(message);
  PC("\"}");
  output->write(output, buf);
  pstring_free(buf);
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
_parse_command(sit_protocol_parser *parser, pstring *pstr) {
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
_line_consume(sit_protocol_parser *parser, pstring *pstr) {
  sit_protocol_handler *handler = parser->handler;
  char *buf;
  pstring tmp = {
    pstr->val,
    pstr->len
  };
  while ((buf = memchr(tmp.val, '\n', tmp.len))) {
    tmp.len = buf - tmp.val;
    if(tmp.val[0] == '{' || parser->state == FORCE_DATA) {
      handler->data_found(handler, &tmp);
      handler->data_complete(handler);
      parser->state = COMPLETE;
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
_line_end_stream(sit_protocol_parser *parser) {
  (void) parser;
}

void 
command_ack(struct sit_protocol_handler *handler, pstring *cmd) {
  sit_protocol_parser *parser = handler->parser;
  sit_input *input = handler->data;
  sit_output *output = input->output;
  parser->data = pcpy(cmd);
  pstring *buf = pstring_new(0);
  PV("{\"status\": \"ok\", \"message\": \"ack\", \"command\": \"%.*s\"}", cmd->len, cmd->val);
  output->write(output, buf);
  pstring_free(buf);    
}

void
_input_command_found(struct sit_protocol_handler *handler, pstring *command, pstring *more) {
  INFO("found cmd:  %.*s\n", command->len, command->val);
  sit_input *input = handler->data;
  sit_output *output = input->output;
  
  if(!cpstrcmp("register", command)) {
    input->qparser_mode = REGISTERING;
    INFO("registering: %.*s\n", more->len, more->val);
    query_parser_consume(input->qparser, more);
    command_ack(handler, command);
  } else if(!cpstrcmp("query", command)) {
    input->qparser_mode = QUERYING;
    query_parser_consume(input->qparser, more);
    command_ack(handler, command);
  } else if(!cpstrcmp("unregister", command)) {
    sit_engine_unregister(input->engine, strtol(more->val, NULL, 10));
    command_ack(handler, command);
  } else if(!cpstrcmp("get", command)) {
    long doc_id = strtol(more->val, NULL, 10);
    pstring *doc = sit_engine_get_document(input->engine, doc_id);
    pstring *buf = pstring_new(0);
    if(doc) {
      PV("{\"status\": \"ok\", \"message\": \"get success\", \"doc\": %.*s}", doc->len, doc->val);
    } else {
      PV("{\"status\": \"error\", \"message\": \"not found\", \"doc_id\": %ld}", doc_id);
    }
    output->write(output, buf);
    pstring_free(buf);
    command_ack(handler, command);
  } else if(!cpstrcmp("close", command)) {
    command_ack(handler, command);
    input->output->close(input->output);
#ifdef HAVE_EV_H
  } else if(isTestMode() && !cpstrcmp("stop", command)) {
    command_ack(handler, command);
    INFO("stopping now!\n");
    ev_unloop(ev_default_loop(0), EVUNLOOP_ALL);
    INFO("stopped\n");
#endif
  } else {
    pstring *buf = pstring_new(0);
    PV("Unknown command: %.*s", command->len, command->val);
    _input_error_found(handler, buf);
    pstring_free(buf);
  }
}

void
_input_data_found(struct sit_protocol_handler *handler, pstring *data) {
  DEBUG("found data: %.*s\n", data->len, data->val);
  sit_input *input = handler->data;
  sit_input_consume(input, data);
}

void
_input_data_complete(struct sit_protocol_handler *handler) {
  (void) handler;
}

sit_protocol_parser *
sit_line_input_protocol_new(sit_input *input) {
  sit_protocol_parser * parser = sit_line_protocol_new();
  sit_protocol_handler *handler = parser->handler;
  parser->data = NULL;
  handler->data = input;
  handler->command_found = _input_command_found;
  handler->data_found    = _input_data_found   ;
  handler->data_complete = _input_data_complete;
  handler->error_found   = _input_error_found  ;
  return parser;
}

sit_protocol_parser *
sit_line_protocol_new() {
  sit_protocol_parser *parser = malloc(sizeof(sit_protocol_parser));
  sit_protocol_handler *handler = malloc(sizeof(sit_protocol_handler));
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
