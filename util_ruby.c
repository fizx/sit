#include "util_ruby.h"

void *
vwrap(VALUE value) {
	value_holder *h = malloc(sizeof(value_holder));
	h->val = value;
	return h;
}

VALUE
vunwrap(void *vp) {
	value_holder *h = (value_holder *) vp;
	return h->val;
}
