#include "ring_buffer.h"
#include <stdlib.h>
#include <limits.h>

ring_buffer *
ring_buffer_new(long capacity) {
	ring_buffer *buffer = malloc(sizeof(ring_buffer));
	buffer->buffer = calloc(1, capacity);
	buffer->capacity = capacity;
	buffer->offset = 0;
	buffer->written = 0;
	return buffer;
}

long
ring_cursor_document_id(sit_cursor *scursor) {
  ring_buffer_cursor *cursor = (ring_buffer_cursor *)scursor;
  return cursor->pos;
}

bool
ring_cursor_prev(sit_cursor *scursor) {
  ring_buffer_cursor *cursor = (ring_buffer_cursor *)scursor;
  long max = cursor->rb->written / cursor->width;
  if(cursor->pos > max) {
    cursor->pos = max;
  }
  cursor->pos--;
  return (cursor->as_cursor.data = ring_buffer_get(cursor->rb, cursor->pos * cursor->width, cursor->width));
}

bool
ring_cursor_next(sit_cursor *scursor) {
  ring_buffer_cursor *cursor = (ring_buffer_cursor *)scursor;
  bool out = ++cursor->pos < (cursor->rb->written - cursor->rb->capacity);
  cursor->as_cursor.data = ring_buffer_get(cursor->rb, cursor->pos * cursor->width, cursor->width);
  return out;
}

void *
ring_cursor_entry(sit_cursor *scursor) {
  ring_buffer_cursor *cursor = (ring_buffer_cursor *)scursor;
  return ring_buffer_get(cursor->rb, cursor->pos * cursor->width, cursor->width);
}

long 
ring_cursor_seek_lte(sit_cursor *scursor, long value) {
  long doc;
	while((doc = ring_cursor_document_id(scursor)) > value) {
		if(!ring_cursor_prev(scursor)) {
			doc = -1;
			break;
		}
	}	
	return doc;
}

ring_buffer_cursor *
ring_buffer_cursor_new(ring_buffer *rb, long width) {
  ring_buffer_cursor *cursor = malloc(sizeof(*cursor));
  cursor->rb = rb;
  cursor->as_cursor.prev = ring_cursor_prev;
  cursor->as_cursor.next = ring_cursor_next;
  cursor->as_cursor.id = ring_cursor_document_id;
  cursor->as_cursor.seek_lte = ring_cursor_seek_lte;
  cursor->as_cursor.data = NULL;
  cursor->width = width;
  cursor->pos = LONG_MAX;
  return cursor;
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
		 len > rb->capacity || off < 0) {
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
	ring_buffer_append(rb, (void *)pstr->val, pstr->len);
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
