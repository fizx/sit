#ifndef RING_BUFFER_H_INCLUDED
#define RING_BUFFER_H_INCLUDED

#include <stdbool.h>
#include "cursor.h"
#include "pstring.h"

typedef struct RingBuffer {
	long capacity;
	long offset;
	long written;
	char *buffer;
} RingBuffer;

typedef struct ring_buffer_cursor {
  struct Cursor as_cursor;
  RingBuffer *rb;
  long width;
  long pos;
  char operator;
  int predicate;
} ring_buffer_cursor;

ring_buffer_cursor *
ring_buffer_cursor_new(RingBuffer *rb, long width);

RingBuffer *
ring_buffer_new(long size);

void
ring_buffer_free(RingBuffer *rb);

void
ring_buffer_append(RingBuffer *rb, void *obj, int len);

void
ring_buffer_reset(RingBuffer *rb);

void *
ring_buffer_get(RingBuffer *rb, long off, int len);

void
ring_buffer_put(RingBuffer *rb, long off, void *obj, int len);

void
ring_buffer_append_pstring(void *rb, pstring *pstr);

pstring *
ring_buffer_get_pstring(void *rb, long off, int len);

ring_buffer_cursor *
ring_buffer_predicate_int_cursor_new(RingBuffer *rb, long width, char operator, int predicate);

#endif
