#include "ring_buffer.h"
#include <stdlib.h>

ring_buffer *
ring_buffer_new(long capacity) {
	ring_buffer *buffer = malloc(sizeof(ring_buffer));
	buffer->buffer = calloc(1, capacity);
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
ring_buffer_reset(ring_buffer *rb) {
	rb->offset = 0;
	rb->written = 0;	
}

void
ring_buffer_append(ring_buffer *rb, void *obj, int len) {	
  ring_buffer_put(rb, rb->written, obj, len);
}

void
ring_buffer_put(ring_buffer *rb, long off, void *obj, int len) {
  if(off < rb->written - rb->capacity) {
    return;
  }
  rb->offset = off % rb->capacity;
  char * chars = obj;
	for (int i = 0; i < len; i++) {
		rb->buffer[rb->offset++] = chars[i];
		if(rb->offset == rb->capacity) {
			rb->offset = 0;
		}
	}
  long end = off + len;
  if(end > rb->written) {
    rb->written = end;
  }
}

void *
ring_buffer_get(ring_buffer *rb, long off, int len) {
	if(off + len > rb->written ||
		 off < rb->written - rb->capacity ||
		 len > rb->capacity) {
		return NULL;
	} else {
		off = off % rb->capacity;
		char *out = malloc(len);
		for (int i = 0; i < len; i++) {
			out[i] = rb->buffer[(off+i) % rb->capacity];
		}
		return out;
	}	
}

void
ring_buffer_append_pstring(void *data, pstring *pstr) {
  ring_buffer *rb = data;
	ring_buffer_append(rb, pstr->val, pstr->len);
}

pstring *
ring_buffer_get_pstring(void *data, long off, int len) {
  ring_buffer *rb = data;
	char *val = ring_buffer_get(rb, off, len);
	if(val == NULL) {
		return NULL;
	} else {
		pstring *pstr = malloc(sizeof(pstring));
		pstr->val = val;
		pstr->len = len;
		return pstr;
	}
}
