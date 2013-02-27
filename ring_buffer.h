#ifndef RING_BUFFER_H_INCLUDED
#define RING_BUFFER_H_INCLUDED

#include <stdbool.h>
#include "cursor.h"
#include "pstring.h"

typedef struct ring_buffer {
	long capacity;
	long offset;
	long written;
	char *buffer;
} ring_buffer;

typedef struct ring_buffer_cursor {
  struct Cursor as_cursor;
  ring_buffer *rb;
  long width;
  long pos;
  char operator;
  int predicate;
} ring_buffer_cursor;

ring_buffer_cursor *
ring_buffer_cursor_new(ring_buffer *rb, long width);

ring_buffer *
ring_buffer_new(long size);

void
ring_buffer_free(ring_buffer *rb);

void
ring_buffer_append(ring_buffer *rb, void *obj, int len);

void
ring_buffer_reset(ring_buffer *rb);

void *
ring_buffer_get(ring_buffer *rb, long off, int len);

void
ring_buffer_put(ring_buffer *rb, long off, void *obj, int len);

void
ring_buffer_append_pstring(void *rb, pstring *pstr);

pstring *
ring_buffer_get_pstring(void *rb, long off, int len);

ring_buffer_cursor *
ring_buffer_predicate_int_cursor_new(ring_buffer *rb, long width, char operator, int predicate);

#endif
