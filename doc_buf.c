#include "sit.h"

void
_db_term_found(struct Receiver *receiver, pstring *pstr, int field_offset) {
  DocBuf *buffer = (DocBuf *)receiver;
	Term *term = &buffer->terms[buffer->term_count++];
	term->field = buffer->field;
  term->text = pcpy(pstr);
	term->offset = field_offset;
	term_update_hash(term);
	dictAdd(buffer->term_index, term, term);  
}

void
_db_document_found(struct Receiver *receiver, pstring *pstr) {
  DocBuf *buffer = (DocBuf *)receiver;
  
  Receiver *r = buffer->receiver;
 
  dictIterator *iter = dictGetIterator(buffer->ints);
  dictEntry *entry;
 
  while ((entry = dictNext(iter))) {
    r->field_found(r, dictGetKey(entry));
    r->int_found(r, dictGetSignedIntegerVal(entry));
  }
 
  dictReleaseIterator(iter);
  
  iter = dictGetIterator(buffer->term_index);
  
  while ((entry = dictNext(iter))) {
    Term *term = dictGetKey(entry);
    r->field_found(r, term->field);
    r->term_found(r, term->text, term->offset);
  }
  
  r->document_found(r, pstr);
}

void
_db_field_found(struct Receiver *receiver, pstring *name) {
  DocBuf *buffer = (DocBuf *)receiver;
  buffer->field = name;
}

void
_db_error_found(struct Receiver *receiver, pstring *message) {
  DocBuf *buffer = (DocBuf *)receiver;
  buffer->receiver->error_found(buffer->receiver, message);
}

void
_db_int_found(struct Receiver *receiver, int value) {
  DocBuf *buffer = (DocBuf *)receiver;
	dictEntry *entry = dictReplaceRaw(buffer->ints, buffer->field);
	dictSetSignedIntegerVal(entry, value);
}

DocBuf *
doc_buf_new(int term_capacity) {
  DocBuf *buf = calloc(1, sizeof(DocBuf) + (term_capacity - 1) * (sizeof(Term)));
  buf->as_receiver.term_found     = _db_term_found;
  buf->as_receiver.document_found = _db_document_found;
  buf->as_receiver.field_found    = _db_field_found;
  buf->as_receiver.error_found    = _db_error_found;
  buf->as_receiver.int_found      = _db_int_found;
  buf->term_capacity = term_capacity;
  buf->term_index = dictCreate(getTermDict(), 0);
	buf->ints = dictCreate(getPstrDict(), 0);
  buf->field = NULL;
  return buf;
}