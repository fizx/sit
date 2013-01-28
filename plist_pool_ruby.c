#include "plist_pool_ruby.h"

VALUE
rbc_plist_pool_new(VALUE class, VALUE rsize) {
	long size = NUM2LONG(rsize);
	plist_pool *pool = plist_pool_new(size);
	VALUE tdata = Data_Wrap_Struct(class, NULL, NULL, pool);
	rb_obj_call_init(tdata, 0, NULL);
	return tdata;
}