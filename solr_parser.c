#include "sit.h"

typedef struct SolrState {
  vstring *stream;
  DocBuf  *doc_buf;
} SolrState;

void 
_solr_consume(Parser *parser, pstring *pstr) {
  SolrState *state = parser->state;
  vstring_append(state->stream, pstr);
  pstring line;
  while (vstring_gets(&line, state->stream)) {
    
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
