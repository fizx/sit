#include "util_ruby.h"
#include "dict.h"

value_holder root = {
  0, 
  NULL
};

const void *
vwrap(VALUE value) {
	value_holder *h = malloc(sizeof(value_holder));
	h->val = value;
  h->next = root.next;
  root.next = h;
	return h;
}

VALUE
vunwrap(const void *vp) {
	value_holder *h = (value_holder *) vp;
	return h->val;
}


void 
markall() {
  value_holder *h = &root;
  while(h->val) {
    rb_gc_mark(h->val);
    h = h->next;
  }
}
