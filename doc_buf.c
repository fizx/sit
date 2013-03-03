#include "sit.h"

#define TERM_CAPACITY 10000

void
doc_buf_term_found(DocBuf *buffer, pstring *pstr, int field_offset) {
  DEBUG("term found: %.*s (%d)", pstr->len, pstr->val, field_offset);
  if(buffer->field) {
  	Term *term = &buffer->terms[buffer->term_count++];
  	term->field = buffer->field;
    term->text = pcpy(pstr);
  	term->offset = field_offset;
  	term_update_hash(term);
  	dictAdd(buffer->term_index, term, term);  
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
  buffer->field = pcpy(name);
}

void
doc_buf_int_found(DocBuf *buffer, int value) {
  if(buffer->field) {
  	dictEntry *entry = dictReplaceRaw(buffer->ints, buffer->field);
  	dictSetSignedIntegerVal(entry, value);
  }
}

void
doc_buf_reset(DocBuf *buf) {
  dictEmpty(buf->ints);
  dictEmpty(buf->term_index);
  buf->field = NULL;  
  buf->term_count = 0;
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

DocBuf *
doc_buf_copy(DocBuf *buf) {
  DocBuf *copy = doc_buf_new();
  copy->field = pcpy(buf->field);
  dictIterator *iterator = dictGetIterator(buf->ints);
	dictEntry *next;
	while((next = dictNext(iterator))) {
    pstring *pstr = pcpy(dictGetKey(next));
    dictEntry *entry = dictReplaceRaw(copy->ints, pstr);
  	dictSetSignedIntegerVal(entry, dictGetSignedIntegerVal(next));
	}
  dictReleaseIterator(iterator);
  iterator = dictGetIterator(buf->term_index);
	while((next = dictNext(iterator))) {
    Term *term = dictGetKey(next);
    doc_buf_field_found(copy, term->field);
    doc_buf_term_found(copy, term->text, term->offset);
	}
  dictReleaseIterator(iterator);
	
  return copy;
}
