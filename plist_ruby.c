#include "plist_ruby.h"
#include <assert.h>

VALUE
rbc_plist_pool_new(VALUE class, VALUE rsize) {
	long size = NUM2LONG(rsize);
	plist_pool *pool = plist_pool_new(size);
	VALUE tdata = Data_Wrap_Struct(class, NULL, NULL, pool);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE
rbc_plist_pool_capacity(VALUE self) {	
	plist_pool *pool;
	Data_Get_Struct(self, plist_pool, pool);
	return LONG2NUM(pool->capacity);
}

VALUE 
rbc_plist_new(VALUE class, VALUE rpool) {
	plist_pool *pool;
	Data_Get_Struct(rpool, plist_pool, pool);
	plist *pl = plist_new(pool);
	VALUE tdata = Data_Wrap_Struct(class, NULL, plist_free, pl);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE
rbc_plist_free(VALUE self) {
	plist *pl;
	Data_Get_Struct(self, plist, pl);
	plist_free(pl);
	rb_iv_set(self, "freed", Qtrue);
	return Qnil;	
}

VALUE
rbc_plist_size(VALUE self) {
	if (rb_iv_get(self, "freed") == Qtrue) {
		rb_raise(rb_eRuntimeError, "already freed");
		return Qnil;
	}
	plist *pl;
	Data_Get_Struct(self, plist, pl);
	return INT2NUM(plist_size(pl));
}

VALUE 
rbc_plist_entry_new(VALUE class, VALUE rdoc, VALUE rpos) {
	int doc = NUM2INT(rdoc);
	int pos = NUM2INT(rpos);
	plist_entry *entry = malloc(sizeof(plist_entry));
	entry->doc = doc;
	entry->pos = pos;
	VALUE tdata = Data_Wrap_Struct(class, NULL, NULL, entry);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE
rbc_plist_append(VALUE self, VALUE rentry) {
	if (rb_iv_get(self, "freed") == Qtrue) {
		rb_raise(rb_eRuntimeError, "already freed");
		return Qnil;
	}
	plist_entry *entry;
	Data_Get_Struct(rentry, plist_entry, entry);
	plist *pl;
	Data_Get_Struct(self, plist, pl);
	plist_append_entry(pl, entry);
	return Qnil;
}

void
_rb_each(void *ventry, void *data) {	
	plist_entry *entry = ventry;
	(void) data;
	plist_entry *dup = malloc(sizeof(plist_entry));
	memcpy(dup, entry, sizeof(plist_entry));
	VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::PlistEntry"), NULL, NULL, dup);
	rb_obj_call_init(tdata, 0, NULL);
	rb_yield(tdata);
}

VALUE
rbc_plist_each(VALUE self) {
	if (rb_iv_get(self, "freed") == Qtrue) {
		rb_raise(rb_eRuntimeError, "already freed");
		return Qnil;
	}
	if (rb_block_given_p()) { 
		plist *pl;
		Data_Get_Struct(self, plist, pl);
		sit_callback iterator;
		iterator.handler = _rb_each;
		plist_reach(pl, &iterator);
	}
	return Qnil;	
}

VALUE
rbc_plist_new_cursor(VALUE self) {
  plist *pl;
	Data_Get_Struct(self, plist, pl);
  assert(pl);
  plist_cursor *cursor = plist_cursor_new(pl);
  if (cursor == NULL) {
    return Qnil;
  } else {
   	VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::PlistCursor"), NULL, NULL, cursor);
  	rb_obj_call_init(tdata, 0, NULL);
    return tdata;
  }
}

VALUE
rbc_plist_cursor_prev(VALUE self) {
  plist_cursor *cursor;
	Data_Get_Struct(self, plist_cursor, cursor);
  return plist_cursor_prev(cursor) ? Qtrue : Qfalse;
}

VALUE
rbc_plist_cursor_next(VALUE self) {
  plist_cursor *cursor;
	Data_Get_Struct(self, plist_cursor, cursor);
  return plist_cursor_next(cursor) ? Qtrue : Qfalse;
}

VALUE
rbc_plist_cursor_entry(VALUE self) {
  plist_cursor *cursor;
	Data_Get_Struct(self, plist_cursor, cursor);
  plist_entry *entry = plist_cursor_entry(cursor);
  if(entry == NULL) {
    return Qnil;
  } else {
    plist_entry *dup = malloc(sizeof(plist_entry));
  	memcpy(dup, entry, sizeof(plist_entry));
    VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::PlistEntry"), NULL, NULL, dup);
  	rb_obj_call_init(tdata, 0, NULL);
    return tdata;
  }
}

VALUE 
rbc_plist_entry_to_s(VALUE self) {
	plist_entry *entry;
	Data_Get_Struct(self, plist_entry, entry);
	char *val;
	asprintf(&val, "<%d:%d>", entry->doc, entry->pos);
	VALUE out = rb_str_new2(val);
	free(val);
	return out;
}

VALUE 
rbc_plist_entry_equals(VALUE self, VALUE other) {
	plist_entry *a;
	Data_Get_Struct(self, plist_entry, a);
	plist_entry *b;
	Data_Get_Struct(other, plist_entry, b);
	return (a->doc == b->doc && a->pos == b->pos) ? Qtrue : Qfalse;
}

VALUE 
rbc_plist_ptr(VALUE self) {
	if (rb_iv_get(self, "freed") == Qtrue) {
		rb_raise(rb_eRuntimeError, "already freed");
		return Qnil;
	}
	plist *pl;
	Data_Get_Struct(self, plist, pl);
	return LONG2NUM((long) pl);
}

VALUE
rbc_plist_region(VALUE self) {
	if (rb_iv_get(self, "freed") == Qtrue) {
		rb_raise(rb_eRuntimeError, "already freed");
		return Qnil;
	}
	plist *pl;
	Data_Get_Struct(self, plist, pl);
	if(plist_size(pl) == 0) {
		return Qnil;
	} else {
		long offset = ((char*) pl->last_block - (char*) (pl->pool->buffer));
		return LONG2NUM(offset / pl->pool->region_size);
	}
}


VALUE 
rbc_plist_blocks_count(VALUE self) {
	if (rb_iv_get(self, "freed") == Qtrue) {
		rb_raise(rb_eRuntimeError, "already freed");
		return Qnil;
	}
	plist *pl;
	Data_Get_Struct(self, plist, pl);
	int min = pl->pool->min_version;
	int count = 0;
	if(pl->last_version >= min) {
		plist_block *block = pl->last_block;
		while(block) {
			count++;
			if(block->prev_version >= min) {
				block = block->prev;
			} else {
				break;
			}
		}
	}
	return INT2NUM(count);
}
