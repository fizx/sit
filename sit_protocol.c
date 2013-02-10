#include "sit_protocol.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define COMMAND_LIMIT 16

pstring *
err_rsp(char *cstr) {
  char *msg;
  asprintf(&msg, "{\"status\": \"error\", \"message\": \"%s\"}", cstr);
  pstring *pstr = c2pstring(msg);
  free(msg);
  return pstr;
}

int
extract_string(pstring *target, pstring *haystack, int off) {
  for (int i = off; i < haystack->len; i++) {
    if (haystack->val[i] == ' ') {
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
  pstring args[COMMAND_LIMIT];
  pstring cmd;
  int off = 0;
  int i;
  off = extract_string(&cmd, pstr, off);
  if(!cmd.len) return; // empty line
  for(i = 0; i < COMMAND_LIMIT && off < pstr->len; i++) {
    off = extract_string(&args[i], pstr, off);
  }
  parser->handler->command_found(parser->handler, &cmd, i, &args);
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
    }
  }
}

void
_line_end_stream(sit_protocol_parser *parser) {
  (void) parser;
}

sit_protocol_parser *
sit_line_protocol_new();

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