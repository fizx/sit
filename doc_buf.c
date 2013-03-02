#include "sit.h"

#define TERM_CAPACITY 10000

void
doc_buf_term_found(DocBuf *buffer, pstring *pstr, int field_offset) {
  DEBUG("term found: %.*s (%d)", pstr->len, pstr->val, field_offset);
	Term *term = &buffer->terms[buffer->term_count++];
	term->field = buffer->field;
  term->text = pcpy(pstr);
	term->offset = field_offset;
	term_update_hash(term);
	dictAdd(buffer->term_index, term, term);  
}

void
doc_buf_doc_found(DocBuf *buffer, pstring *pstr) {
  DEBUG("doc found: %.*s", pstr->len, pstr->val);
  buffer->doc = pstr;
}

void
doc_buf_field_found(DocBuf *buffer, pstring *name) {
  DEBUG("field found: %.*s", name->len, name->val);
  buffer->field = name;
}

void
doc_buf_int_found(DocBuf *buffer, int value) {
  DEBUG("int found: %d", value);
  assert(buffer->field);
	dictEntry *entry = dictReplaceRaw(buffer->ints, buffer->field);
	dictSetSignedIntegerVal(entry, value);
}

DocBuf *
doc_buf_new() {
  DocBuf *buf = calloc(1, sizeof(DocBuf) + (TERM_CAPACITY - 1) * (sizeof(Term)));
  buf->term_capacity = TERM_CAPACITY;
  buf->term_index = dictCreate(getTermDict(), 0);
	buf->ints = dictCreate(getPstrDict(), 0);
  buf->field = NULL;
  return buf;
}