#ifndef SIT_RECEIVER_H_INCLUDED
#define SIT_RECEIVER_H_INCLUDED

#include "pstring.h"

struct sit_parser;

typedef struct sit_receiver {
	void (*term_found)(struct sit_receiver *receiver, long off, int len, int field_offset);
  void (*document_found)(struct sit_receiver *receiver, long off, int len);
  void (*field_found)(struct sit_receiver *receiver, pstring *name);
  void (*error_found)(struct sit_receiver *receiver, pstring *message);
  void (*int_found)(struct sit_receiver *receiver, int val);
	void *data;  // for user purposes, e.g. the ruby bindings keep the ruby object here.
} sit_receiver;


sit_receiver *
sit_receiver_new();

#endif

