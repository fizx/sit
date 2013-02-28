#include "sit.h"

void
doc_buf_term_found(DocBuf *buffer, pstring *pstr, int field_offset) {
	Term *term = &buffer->terms[buffer->term_count++];
	term->field = buffer->field;
  term->text = pcpy(pstr);
	term->offset = field_offset;
	term_update_hash(term);
	dictAdd(buffer->term_index, term, term);  
}

void
doc_buf_document_found(DocBuf *buffer, pstring *pstr) {
  buffer->doc = pstr;
}

void
doc_buf_field_found(DocBuf *buffer, pstring *name) {
  buffer->field = name;
}

void
doc_buf_int_found(DocBuf *buffer, int value) {
	dictEntry *entry = dictReplaceRaw(buffer->ints, buffer->field);
	dictSetSignedIntegerVal(entry, value);
}

DocBuf *
doc_buf_new(int term_capacity) {
  DocBuf *buf = calloc(1, sizeof(DocBuf) + (term_capacity - 1) * (sizeof(Term)));
  buf->term_capacity = term_capacity;
  buf->term_index = dictCreate(getTermDict(), 0);
	buf->ints = dictCreate(getPstrDict(), 0);
  buf->field = NULL;
  return buf;
}