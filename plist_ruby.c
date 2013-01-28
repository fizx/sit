#include "plist_ruby.h"

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
	VALUE tdata = Data_Wrap_Struct(class, NULL, NULL, pl);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}

VALUE
rbc_plist_size(VALUE self) {
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
	plist_entry *entry;
	Data_Get_Struct(rentry, plist_entry, entry);
	plist *pl;
	Data_Get_Struct(self, plist, pl);
	plist_append_entry(pl, entry);
	return Qnil;
}

void
_rb_each(void *data, plist_entry *entry) {	
	(void) data;
	plist_entry *dup = malloc(sizeof(plist_entry));
	memcpy(dup, entry, sizeof(plist_entry));
	VALUE tdata = Data_Wrap_Struct(rb_eval_string("::Sit::PlistEntry"), NULL, NULL, dup);
	rb_obj_call_init(tdata, 0, NULL);
	rb_yield(tdata);
}

VALUE
rbc_plist_each(VALUE self) {
	if (rb_block_given_p()) { 
		plist *pl;
		Data_Get_Struct(self, plist, pl);
		plist_iterator iterator;
		iterator.handle = _rb_each;
		plist_reach(pl, &iterator);
	}
	return Qnil;	
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

