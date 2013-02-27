#ifndef SIT_RECEIVER_H_INCLUDED
#define SIT_RECEIVER_H_INCLUDED

#include "pstring.h"

struct Parser;

typedef struct Receiver {
	void (*term_found)(struct Receiver *receiver, pstring *pstr, int field_offset);
  void (*document_found)(struct Receiver *receiver, long off, int len);
  void (*field_found)(struct Receiver *receiver, pstring *name);
  void (*error_found)(struct Receiver *receiver, pstring *message);
  void (*int_found)(struct Receiver *receiver, int val);
	void *data;  // for user purposes, e.g. the ruby bindings keep the ruby object here.
} Receiver;


Receiver *
sit_receiver_new();

#endif

