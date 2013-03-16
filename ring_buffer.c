#include "sit.h"

RingBuffer *
ring_buffer_new(long capacity) {
	RingBuffer *buffer = malloc(sizeof(RingBuffer));
	buffer->buffer = calloc(1, capacity);
	buffer->capacity = capacity;
	buffer->written = 0;
  buffer->on_evict = NULL;
	return buffer;
}

long
ring_cursor_document_id(Cursor *scursor) {
  ring_buffer_cursor *cursor = (ring_buffer_cursor *)scursor;
  return cursor->pos;
}

bool
ring_cursor_prev(Cursor *scursor) {
  ring_buffer_cursor *cursor = (ring_buffer_cursor *)scursor;
  long max = cursor->rb->written / cursor->width;
  if(cursor->pos > max) {
    cursor->pos = max;
  }
  cursor->pos--;
  return (cursor->as_cursor.data = ring_buffer_get(cursor->rb, cursor->pos * cursor->width, cursor->width));
}

bool
ring_cursor_next(Cursor *scursor) {
  ring_buffer_cursor *cursor = (ring_buffer_cursor *)scursor;
  bool out = ++cursor->pos < (cursor->rb->written - cursor->rb->capacity);
  cursor->as_cursor.data = ring_buffer_get(cursor->rb, cursor->pos * cursor->width, cursor->width);
  return out;
}

void *
ring_cursor_entry(Cursor *scursor) {
  ring_buffer_cursor *cursor = (ring_buffer_cursor *)scursor;
  return ring_buffer_get(cursor->rb, cursor->pos * cursor->width, cursor->width);
}

long 
ring_cursor_seek_lte(Cursor *cursor, long value) {
  long doc;
	while((doc = cursor->id(cursor)) > value) {
		if(!cursor->prev(cursor)) {
			doc = -1;
			break;
		}
	}	
	return doc;
}

//!ruby
//!undent 5
// <% 
//   $ops = [%w[= == eq], %w[> > gt ], %w[< < lt], %w[! != ne], %w[. >= gte], %w[, <= lte]]
//   $ops.each do |char, operator, prefix| 
// %>
//   <% %w[prev next].each do |direction| %>
//     bool                                                                       
//     <%=prefix%>_predicate_ring_cursor_<%=direction%>(Cursor *scursor) {                   
//       ring_buffer_cursor *cursor = (ring_buffer_cursor *) scursor;             
//       while(ring_cursor_<%=direction%>(scursor)){                                        
//         if((*(int*)ring_cursor_entry(scursor)) <%=operator%> cursor->predicate) {  
//           return true;                                                         
//         }                                                                      
//       }                                                                        
//       return false;                                                            
//     }                                                                   
//   <% end %>
// <% end %>


bool                                                                       
eq_predicate_ring_cursor_prev(Cursor *scursor) {                   
  ring_buffer_cursor *cursor = (ring_buffer_cursor *) scursor;             
  while(ring_cursor_prev(scursor)){                                        
    if((*(int*)ring_cursor_entry(scursor)) == cursor->predicate) {  
      return true;                                                         
    }                                                                      
  }                                                                        
  return false;                                                            
}                                                                   
   
bool                                                                       
eq_predicate_ring_cursor_next(Cursor *scursor) {                   
  ring_buffer_cursor *cursor = (ring_buffer_cursor *) scursor;             
  while(ring_cursor_next(scursor)){                                        
    if((*(int*)ring_cursor_entry(scursor)) == cursor->predicate) {  
      return true;                                                         
    }                                                                      
  }                                                                        
  return false;                                                            
}                                                                   

bool                                                                       
gt_predicate_ring_cursor_prev(Cursor *scursor) {                   
  ring_buffer_cursor *cursor = (ring_buffer_cursor *) scursor;             
  while(ring_cursor_prev(scursor)){                                        
    if((*(int*)ring_cursor_entry(scursor)) > cursor->predicate) {  
      return true;                                                         
    }                                                                      
  }                                                                        
  return false;                                                            
}                                                                   
   
bool                                                                       
gt_predicate_ring_cursor_next(Cursor *scursor) {                   
  ring_buffer_cursor *cursor = (ring_buffer_cursor *) scursor;             
  while(ring_cursor_next(scursor)){                                        
    if((*(int*)ring_cursor_entry(scursor)) > cursor->predicate) {  
      return true;                                                         
    }                                                                      
  }                                                                        
  return false;                                                            
}                                                                   

bool                                                                       
lt_predicate_ring_cursor_prev(Cursor *scursor) {                   
  ring_buffer_cursor *cursor = (ring_buffer_cursor *) scursor;             
  while(ring_cursor_prev(scursor)){                                        
    if((*(int*)ring_cursor_entry(scursor)) < cursor->predicate) {  
      return true;                                                         
    }                                                                      
  }                                                                        
  return false;                                                            
}                                                                   
   
bool                                                                       
lt_predicate_ring_cursor_next(Cursor *scursor) {                   
  ring_buffer_cursor *cursor = (ring_buffer_cursor *) scursor;             
  while(ring_cursor_next(scursor)){                                        
    if((*(int*)ring_cursor_entry(scursor)) < cursor->predicate) {  
      return true;                                                         
    }                                                                      
  }                                                                        
  return false;                                                            
}                                                                   

bool                                                                       
ne_predicate_ring_cursor_prev(Cursor *scursor) {                   
  ring_buffer_cursor *cursor = (ring_buffer_cursor *) scursor;             
  while(ring_cursor_prev(scursor)){                                        
    if((*(int*)ring_cursor_entry(scursor)) != cursor->predicate) {  
      return true;                                                         
    }                                                                      
  }                                                                        
  return false;                                                            
}                                                                   
   
bool                                                                       
ne_predicate_ring_cursor_next(Cursor *scursor) {                   
  ring_buffer_cursor *cursor = (ring_buffer_cursor *) scursor;             
  while(ring_cursor_next(scursor)){                                        
    if((*(int*)ring_cursor_entry(scursor)) != cursor->predicate) {  
      return true;                                                         
    }                                                                      
  }                                                                        
  return false;                                                            
}                                                                   

bool                                                                       
gte_predicate_ring_cursor_prev(Cursor *scursor) {                   
  ring_buffer_cursor *cursor = (ring_buffer_cursor *) scursor;             
  while(ring_cursor_prev(scursor)){                                        
    if((*(int*)ring_cursor_entry(scursor)) >= cursor->predicate) {  
      return true;                                                         
    }                                                                      
  }                                                                        
  return false;                                                            
}                                                                   
   
bool                                                                       
gte_predicate_ring_cursor_next(Cursor *scursor) {                   
  ring_buffer_cursor *cursor = (ring_buffer_cursor *) scursor;             
  while(ring_cursor_next(scursor)){                                        
    if((*(int*)ring_cursor_entry(scursor)) >= cursor->predicate) {  
      return true;                                                         
    }                                                                      
  }                                                                        
  return false;                                                            
}                                                                   

bool                                                                       
lte_predicate_ring_cursor_prev(Cursor *scursor) {                   
  ring_buffer_cursor *cursor = (ring_buffer_cursor *) scursor;             
  while(ring_cursor_prev(scursor)){                                        
    if((*(int*)ring_cursor_entry(scursor)) <= cursor->predicate) {  
      return true;                                                         
    }                                                                      
  }                                                                        
  return false;                                                            
}                                                                   
   
bool                                                                       
lte_predicate_ring_cursor_next(Cursor *scursor) {                   
  ring_buffer_cursor *cursor = (ring_buffer_cursor *) scursor;             
  while(ring_cursor_next(scursor)){                                        
    if((*(int*)ring_cursor_entry(scursor)) <= cursor->predicate) {  
      return true;                                                         
    }                                                                      
  }                                                                        
  return false;                                                            
}
//!end

ring_buffer_cursor *
ring_buffer_predicate_int_cursor_new(RingBuffer *rb, long width, char operator, int predicate) {
  ring_buffer_cursor *cursor = ring_buffer_cursor_new(rb, width);
  switch (operator) {    

  case '=':
    cursor->as_cursor.prev = eq_predicate_ring_cursor_prev;
    cursor->as_cursor.next = eq_predicate_ring_cursor_next;
    break;

  case '>':
    cursor->as_cursor.prev = gt_predicate_ring_cursor_prev;
    cursor->as_cursor.next = gt_predicate_ring_cursor_next;
    break;

  case '<':
    cursor->as_cursor.prev = lt_predicate_ring_cursor_prev;
    cursor->as_cursor.next = lt_predicate_ring_cursor_next;
    break;

  case '!':
    cursor->as_cursor.prev = ne_predicate_ring_cursor_prev;
    cursor->as_cursor.next = ne_predicate_ring_cursor_next;
    break;

  case '.':
    cursor->as_cursor.prev = gte_predicate_ring_cursor_prev;
    cursor->as_cursor.next = gte_predicate_ring_cursor_next;
    break;

  case ',':
    cursor->as_cursor.prev = lte_predicate_ring_cursor_prev;
    cursor->as_cursor.next = lte_predicate_ring_cursor_next;
    break;
  //!end
  }
  cursor->operator = operator;
  cursor->predicate = predicate;
  return cursor;
}

ring_buffer_cursor *
ring_buffer_cursor_new(RingBuffer *rb, long width) {
  ring_buffer_cursor *cursor = malloc(sizeof(*cursor));
  cursor->rb = rb;
  cursor->as_cursor.prev = ring_cursor_prev;
  cursor->as_cursor.next = ring_cursor_next;
  cursor->as_cursor.id = ring_cursor_document_id;
  cursor->as_cursor.seek_lte = ring_cursor_seek_lte;
  cursor->as_cursor.data = NULL;
  cursor->width = width;
  cursor->pos = rb->written / width;
  return cursor;
}

void
ring_buffer_free(RingBuffer *rb) {
	free(rb->buffer);
	free(rb);
}

void
ring_buffer_reset(RingBuffer *rb) {
	rb->written = 0;	
}

void
ring_buffer_append(RingBuffer *rb, void *obj, int len) {	
  if(rb->on_evict && rb->written >= rb->capacity) {
    rb->on_evict->handler(rb->on_evict, rb->buffer + (rb->written % rb->capacity));
  }
  ring_buffer_put(rb, rb->written, obj, len);
}

void
ring_buffer_put(RingBuffer *rb, long off, void *obj, int len) {
  if(
    off < rb->written - rb->capacity ||
    len > rb->capacity
  ) return;
  
  int localoff = off % rb->capacity;
  int remaining = rb->capacity - localoff;
  int len1 = remaining > len ? len : remaining;
  memcpy(rb->buffer + localoff, obj, len1);
  int more = len - len1;
  if(more) {
    memcpy(rb->buffer, obj + len1, more);
  }
  
  long end = off + len;
  if(end > rb->written) {
    rb->written = end;
  }
}

void *
ring_buffer_get(RingBuffer *rb, long off, int len) {
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
  RingBuffer *rb = data;
	ring_buffer_append(rb, (void *)pstr->val, pstr->len);
}

pstring *
ring_buffer_get_pstring(void *data, long off, int len) {
  RingBuffer *rb = data;
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