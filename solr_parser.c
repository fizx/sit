#include "sit.h"

typedef struct SolrState {
  vstring *stream;
  DocBuf  *doc_buf;
} SolrState;

#define strnchr(a, b)  (a == NULL ? NULL : strchr(a, b))
#define strnstr(a, b)  (a == NULL ? NULL : strstr(a, b))

char * 
pstrnchr(pstring *base, char *start, const char c) {
  if(start<base->val) return NULL;
  int len = base->len - (start - base->val);
  return strnchr(start, c);
}

char * 
pstrnstr(pstring *base, char *start, const char *c) {
  if(start<base->val) return NULL;
  int len = base->len - (start - base->val);
  return strnstr(start, c);
}

pstring _severity = { "severity", 8 };
pstring _core     = { "core", 4 };
pstring _path     = { "path", 4 };
pstring _hits     = { "hits", 4 };
pstring _status   = { "status", 6 };
pstring _qtime    = { "qtime", 5 };

void 
_solr_consume(Parser *parser, pstring *pstr) {
  SolrState *state = parser->state;
  vstring_append(state->stream, pstr);
  pstring line;
  DocBuf *buf = state->doc_buf;
  while (vstring_gets(&line, state->stream)) {
    char *colon  = pstrnchr(&line, line.val, ':');
    char *close  = pstrnchr(&line, colon, ']');
    char *path   = pstrnstr(&line, close, " path=");
    char *closep = pstrnchr(&line, path + 2, ' ');
    char *hits   = pstrnstr(&line, closep, " hits=");
    char *status = pstrnstr(&line, closep, " status=");
    char *qtime  = pstrnstr(&line, status, " QTime=");
    if(qtime) {
      doc_buf_field_found(buf, &_severity);
      pstring severity = { line.val, colon - line.val };
      doc_buf_term_found(buf, &severity, 0);

      doc_buf_field_found(buf, &_core);
      pstring core = { colon + 3, close - colon - 3 };
      doc_buf_term_found(buf, &core, 0);

      doc_buf_field_found(buf, &_path);
      pstring tpath = { path + 6, closep - path - 3 };
      doc_buf_term_found(buf, &tpath, 0);
      
      if(hits) {
        doc_buf_field_found(buf, &_hits);  
        doc_buf_int_found(buf, strtol(hits + 6, NULL, 10));
      }
      
      doc_buf_field_found(buf, &_status);  
      doc_buf_int_found(buf, strtol(status + 8, NULL, 10));
      
      doc_buf_field_found(buf, &_qtime);  
      doc_buf_int_found(buf, strtol(qtime + 7, NULL, 10));
      doc_buf_doc_found(buf, &line);
      parser->on_document->handler(parser->on_document, buf);
      doc_buf_reset(buf);
    }
  }
}

Parser *
solr_parser_new() {
  Parser *parser = parser_new();
  parser->free = solr_parser_free;
  parser->fresh_copy = solr_fresh_copy;
  parser->consume = _solr_consume;
  SolrState *state = malloc(sizeof(SolrState));
  parser->state = state;
  state->stream = vstring_new();
  state->doc_buf = doc_buf_new(100);
  return parser;
}

void
solr_parser_free(void *data) {
  Parser *parser = data;
  SolrState *state = parser->state;
  vstring_free(state->stream);
  doc_buf_free(state->doc_buf);
  free(parser->state);
  free(parser);
}

Parser *
solr_fresh_copy(Parser *parser) {
  return solr_parser_new();
}
