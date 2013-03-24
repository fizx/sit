#include "sit.h"

typedef struct SyslogState {
  vstring   *stream;
  DocBuf    *doc_buf;
  Tokenizer *tok;
} SyslogState;

pstring _time     = { "time", 4 };
pstring _host     = { "host", 4 };
pstring _tag      = { "tag", 3 };
pstring _message  = { "message", 7 };

pstring not_syslog = { "this line doesn't look like syslog", 34 };

void 
_syslog_consume(Parser *parser, pstring *pstr) {
  SyslogState *state = parser->state;
  Tokenizer *tok = state->tok;
  vstring_append(state->stream, pstr);
  pstring line;
  DocBuf *buf = state->doc_buf;
  struct tm now;
  time_t nowstamp = time(NULL);
  gmtime_r(&nowstamp, &now);
  while (vstring_gets(&line, state->stream)) {
    char *endhost  = pstrnchr(&line, line.val + 17, ' ');
    char *colon  = pstrnchr(&line, endhost, ':');
    if(colon) {
      struct tm ts;
      doc_buf_field_found(buf, &_time);
      strptime(line.val, "%b %d %H:%M:%S", &ts);
      ts.tm_year = now.tm_year;
      time_t log_time = timegm(&ts);
      if(log_time > nowstamp) {
        ts.tm_year--;
        log_time = mktime(&ts);
      }
      doc_buf_int_found(buf, log_time);

      doc_buf_field_found(buf, &_host);
      pstring host = { line.val + 16, endhost - (line.val + 16) };
      doc_buf_term_found(buf, &host, 0);
      
      doc_buf_field_found(buf, &_tag);
      pstring tag = { endhost + 1, colon - (endhost + 1) };
      tok->consume(tok, &tag);
      
      doc_buf_field_found(buf, &_message);
      pstring msg = { colon + 1, (line.val + line.len) - (colon + 1) };
      tok->consume(tok, &msg);

      doc_buf_doc_found(buf, &line);
      parser->on_document->handler(parser->on_document, buf);
      doc_buf_reset(buf);
    } else {
      parser->on_error->handler(parser->on_error, &not_syslog);
    }
  }
}

void
_syslog_token_found(Callback *cb, void *data) {
  Token *token = data;
  SyslogState *state = cb->user_data;
  doc_buf_term_found(state->doc_buf, token->text, token->offset);
}

Parser *
syslog_simple_parser_new() {
  Parser *parser = parser_new();
  parser->free = syslog_parser_free;
  parser->fresh_copy = syslog_fresh_copy;
  parser->consume = _syslog_consume;
  SyslogState *state = malloc(sizeof(SyslogState));
  parser->state = state;
  state->tok = regex_tokenizer_new("\\W+");
  state->tok->on_token = callback_new(_syslog_token_found, state);  
  state->stream = vstring_new();
  state->doc_buf = doc_buf_new(500);
  return parser;
}

void
syslog_parser_free(void *data) {
  Parser *parser = data;
  SyslogState *state = parser->state;
  tokenizer_free(state->tok);
  vstring_free(state->stream);
  doc_buf_free(state->doc_buf);
  free(parser->state);
  free(parser);
}

Parser *
syslog_fresh_copy(Parser *parser) {
  return syslog_simple_parser_new();
}
