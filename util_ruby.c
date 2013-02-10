#include "util_ruby.h"
#include "dict.h"
#include <stdbool.h>

value_holder root = {
  0, 
  NULL
};

bool _is_test_mode = false;

bool
isTestMode() {
  return _is_test_mode;
}

void
setTestMode(bool val) {
  _is_test_mode = val;
}

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

VALUE
rbc_isTestMode() {
  return _is_test_mode ? Qtrue : Qfalse;
}

VALUE
rbc_setTestMode(VALUE val) {
  _is_test_mode = val == Qtrue;
  return Qnil;
}