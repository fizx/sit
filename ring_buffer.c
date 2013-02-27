#include "sit.h"

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
ring_buffer_predicate_int_cursor_new(ring_buffer *rb, long width, char operator, int predicate) {
  ring_buffer_cursor *cursor = ring_buffer_cursor_new(rb, width);
  switch (operator) {    
  //!ruby
  // <% $ops.each do |char, operator, prefix| %>
  //   case '<%=char%>':
  //     cursor->as_cursor.prev = <%=prefix%>_predicate_ring_cursor_prev;
  //     cursor->as_cursor.next = <%=prefix%>_predicate_ring_cursor_next;
  //     break;
  // <% end %>


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
ring_buffer_cursor_new(ring_buffer *rb, long width) {
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