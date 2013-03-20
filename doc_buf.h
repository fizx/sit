#ifndef DOC_BUF_H_INCLUDED
#define DOC_BUF_H_INCLUDED

#include "term.h"
#include "dict.h"

typedef struct DocBuf {
	// fields used to manage the current document
  dict            *term_index;
  int              term_count;
  pstring         *field;
  int              term_capacity;
	dict            *ints;
  pstring         *doc;
  unsigned int     hash_code;
  Term             terms[1];
} DocBuf;

DocBuf *
doc_buf_new();

void
doc_buf_reset(DocBuf *buf);

void
doc_buf_set_hash_code(DocBuf *buffer, unsigned int hash);

void
doc_buf_free(DocBuf *buf);

void
doc_buf_term_found(DocBuf *buffer, pstring *pstr, int field_offset);

void
doc_buf_doc_found(DocBuf *buffer, pstring *pstr);

void
doc_buf_field_found(DocBuf *buffer, pstring *name);

void
doc_buf_int_found(DocBuf *buffer, int value);

#endif