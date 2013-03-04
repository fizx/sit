#include "sit.h"

#define TERM_CAPACITY 10000

void
doc_buf_term_found(DocBuf *buffer, pstring *pstr, int field_offset) {
  DEBUG("term found: %.*s (%d)", pstr->len, pstr->val, field_offset);
  if(buffer->field) {
    Term *term = &buffer->terms[buffer->term_count++];
  	term->field = *buffer->field;
    term->text = *pstr;
  	term->offset = field_offset;
  	term_update_hash(term);
    dictAdd(buffer->term_index, term, term);  
	} else {
    WARN("term without field");
	}
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
  if(buffer->field) {
  	dictEntry *entry = dictReplaceRaw(buffer->ints, buffer->field);
  	dictSetSignedIntegerVal(entry, value);
  } else {
    WARN("int without field");
	}
}

void
doc_buf_reset(DocBuf *buf) {
  dictEmpty(buf->ints);
  dictEmpty(buf->term_index);
  buf->field = NULL;  
  buf->doc = NULL;  
  buf->term_count = 0;
}

void
doc_buf_free(DocBuf *buf) {
  dictRelease(buf->ints);
  dictRelease(buf->term_index);
  free(buf);
}

DocBuf *
doc_buf_new() {
  DocBuf *buf = calloc(1, sizeof(DocBuf) + sizeof(Term) * (TERM_CAPACITY - 1));
  buf->term_capacity = TERM_CAPACITY;
  buf->term_index = dictCreate(getTermTermDict(), 0);
	buf->ints = dictCreate(getPstrDict(), 0);
  buf->field = NULL;
  return buf;
}
