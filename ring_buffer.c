#include "ring_buffer.h"
#include <stdlib.h>

ring_buffer *
ring_buffer_new(long capacity) {
	ring_buffer *buffer = malloc(sizeof(ring_buffer));
	buffer->buffer = malloc(capacity);
	buffer->capacity = capacity;
	buffer->offset = 0;
	buffer->written = 0;
	return buffer;
}

void
ring_buffer_free(ring_buffer *rb) {
	free(rb->buffer);
	free(rb);
}

void
ring_buffer_append(ring_buffer *rb, pstring *pstr) {
	for (int i = 0; i < pstr->len; i++) {
		rb->buffer[rb->offset++] = pstr->val[i];
		if(rb->offset == rb->capacity) {
			rb->offset = 0;
		}
	}
	rb->written += pstr->len;
}

pstring *
ring_buffer_get(ring_buffer *rb, long off, int len) {
	if(off + len > rb->written ||
		 off < rb->written - rb->capacity ||
		 len > rb->capacity) {
		return NULL;
	} else {
		off = off % rb->capacity;
		pstring *pstr = pstring_new(len);
		for (int i = 0; i < len; i++) {
			pstr->val[i] = rb->buffer[(off+i) % rb->capacity];
		}
		return pstr;
	}	
}