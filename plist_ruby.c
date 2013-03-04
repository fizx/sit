#include "sit_ruby.h"

VALUE
rbc_plist_pool_new(VALUE class, VALUE rsize) {
	long size = NUM2LONG(rsize);
	PlistPool *pool = plist_pool_new(size);
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, pool);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE
rbc_plist_pool_capacity(VALUE self) {	
	PlistPool *pool;
	Data_Get_Struct(self, PlistPool, pool);
	return LONG2NUM(pool->capacity);
}

VALUE 
rbc_plist_new(VALUE class, VALUE rpool) {
	PlistPool *pool;
	Data_Get_Struct(rpool, PlistPool, pool);
	Plist *pl = plist_new(pool);
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, pl);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE
rbc_plist_free(VALUE self) {
	Plist *pl;
	Data_Get_Struct(self, Plist, pl);
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
	Plist *pl;
	Data_Get_Struct(self, Plist, pl);
	return INT2NUM(plist_size(pl));
}

VALUE 
rbc_plist_entry_new(VALUE class, VALUE rdoc, VALUE rpos) {
	int doc = NUM2INT(rdoc);
	int pos = NUM2INT(rpos);
	PlistEntry *entry = malloc(sizeof(PlistEntry));
	entry->doc = doc;
	entry->pos = pos;
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, entry);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE
rbc_plist_append(VALUE self, VALUE rentry) {
	if (rb_iv_get(self, "freed") == Qtrue) {
		rb_raise(rb_eRuntimeError, "already freed");
		return Qnil;
	}
	PlistEntry *entry;
	Data_Get_Struct(rentry, PlistEntry, entry);
	Plist *pl;
	Data_Get_Struct(self, Plist, pl);
	plist_append_entry(pl, entry);
	return Qnil;
}

void
_rb_each(Callback *cb, void *ventry) {	
	PlistEntry *entry = ventry;
	(void) cb;
	PlistEntry *dup = malloc(sizeof(PlistEntry));
	memcpy(dup, entry, sizeof(PlistEntry));
	VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::PlistEntry"), markall, NULL, dup);
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
		Plist *pl;
		Data_Get_Struct(self, Plist, pl);
		Callback iterator;
		iterator.handler = _rb_each;
		plist_reach(pl, &iterator);
	}
	return Qnil;	
}

VALUE
rbc_plist_new_cursor(VALUE self) {
  Plist *pl;
	Data_Get_Struct(self, Plist, pl);
  assert(pl);
  PlistCursor *cursor = plist_cursor_new(pl);
  if (cursor == NULL) {
    return Qnil;
  } else {
   	VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::PlistCursor"), markall, NULL, cursor);
  	rb_obj_call_init(tdata, 0, NULL);
    return tdata;
  }
}

VALUE
rbc_plist_cursor_prev(VALUE self) {
  PlistCursor *cursor;
	Data_Get_Struct(self, PlistCursor, cursor);
  Cursor *sc = &cursor->as_cursor;
  return sc->prev(sc) ? Qtrue : Qfalse;
}

VALUE
rbc_plist_cursor_next(VALUE self) {
  PlistCursor *cursor;
	Data_Get_Struct(self, PlistCursor, cursor);
	Cursor *sc = &cursor->as_cursor;
  return sc->next(sc) ? Qtrue : Qfalse;
}

VALUE
rbc_plist_cursor_entry(VALUE self) {
  PlistCursor *cursor;
	Data_Get_Struct(self, PlistCursor, cursor);
	Cursor *sc = &cursor->as_cursor;
  PlistEntry *entry = sc->data;
  if(entry == NULL) {
    return Qnil;
  } else {
    PlistEntry *dup = malloc(sizeof(PlistEntry));
  	memcpy(dup, entry, sizeof(PlistEntry));
    VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::PlistEntry"), markall, NULL, dup);
  	rb_obj_call_init(tdata, 0, NULL);
    return tdata;
  }
}

VALUE 
rbc_plist_entry_to_s(VALUE self) {
	PlistEntry *entry;
	Data_Get_Struct(self, PlistEntry, entry);
	char *val;
	asprintf(&val, "<%d:%d>", entry->doc, entry->pos);
	VALUE out = rb_str_new2(val);
	free(val);
	return out;
}

VALUE 
rbc_plist_entry_equals(VALUE self, VALUE other) {
	PlistEntry *a;
	Data_Get_Struct(self, PlistEntry, a);
	PlistEntry *b;
	Data_Get_Struct(other, PlistEntry, b);
	return (a->doc == b->doc && a->pos == b->pos) ? Qtrue : Qfalse;
}

VALUE 
rbc_plist_ptr(VALUE self) {
	if (rb_iv_get(self, "freed") == Qtrue) {
		rb_raise(rb_eRuntimeError, "already freed");
		return Qnil;
	}
	Plist *pl;
	Data_Get_Struct(self, Plist, pl);
	return LONG2NUM((long) pl);
}

VALUE
rbc_plist_region(VALUE self) {
	if (rb_iv_get(self, "freed") == Qtrue) {
		rb_raise(rb_eRuntimeError, "already freed");
		return Qnil;
	}
	Plist *pl;
	Data_Get_Struct(self, Plist, pl);
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
	Plist *pl;
	Data_Get_Struct(self, Plist, pl);
	int min = pl->pool->min_version;
	int count = 0;
	if(pl->last_version >= min) {
		PlistBlock *block = pl->last_block;
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
