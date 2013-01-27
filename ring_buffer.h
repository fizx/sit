#ifndef RING_BUFFER_H_INCLUDED
#define RING_BUFFER_H_INCLUDED

#include "pstring.h"

typedef struct {
	long capacity;
	long offset;
	long written;
	char *buffer;
} ring_buffer;

ring_buffer *
ring_buffer_new(long size);

void
ring_buffer_free(ring_buffer *rb);

void
ring_buffer_append(ring_buffer *rb, pstring *str);

pstring *
ring_buffer_get(ring_buffer *rb, long off, int len);

#endif
