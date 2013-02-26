#ifndef DOC_BUF_H_INCLUDED
#define DOC_BUF_H_INCLUDED

#include "receiver.h"
#include "term.h"

typedef struct DocBuf {
  struct Receiver as_receiver;
  struct Receiver *receiver; // to forward to

	// fields used to manage the current document
  dict            *term_index;
  int              term_count;
  pstring         *field;
  int              term_capacity;
	dict            *ints;

  Term             terms[1];
} DocBuf;

DocBuf *
doc_buf_new(int term_capacity);

#endif