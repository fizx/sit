#include "sit.h"

#define TERM_CAPACITY 10000

void
doc_buf_term_found(DocBuf *buffer, pstring *pstr, int field_offset) {
  DEBUG("term found: %.*s (%d)", pstr->len, pstr->val, field_offset);
  if(buffer->field) {
    Term *term = calloc(1, sizeof(Term));//&buffer->terms[buffer->term_count++];
    buffer->term_count++;
  	term->field = pcpy(buffer->field);
    term->text = pcpy(pstr);
  	term->offset = field_offset;
  	term_update_hash(term);
    if (!dictFetchValue(buffer->term_index, term)) {
  	  dictAdd(buffer->term_index, term, term);  
	  } else {
      term_free(term);
	  }
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
  if(buffer->field) {
    pstring_free(buffer->field);
  }
  buffer->field = pcpy(name);
}

void
doc_buf_int_found(DocBuf *buffer, int value) {
  if(buffer->field) {
  	dictEntry *entry = dictReplaceRaw(buffer->ints, pcpy(buffer->field));
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
  if(buf->field) pstring_free(buf->field);
  if(buf->doc) pstring_free(buf->doc);
  free(buf);
}

DocBuf *
doc_buf_new() {
  DocBuf *buf = calloc(1, sizeof(DocBuf));
  buf->term_capacity = TERM_CAPACITY;
  buf->term_index = dictCreate(getTermTermDict(), 0);
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
