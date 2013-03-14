#include "sit_ruby.h"

value_holder root = {
  0, 
  NULL
};

void *
vwrap(VALUE value) {
	value_holder *h = malloc(sizeof(value_holder));
	h->val = value;
  h->next = root.next;
  root.next = h;
	return h;
}

VALUE
vunwrap(void *vp) {
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
rbc_json_escape(VALUE class, VALUE rstr) {
  pstring *in = r2pstring(rstr);
  pstring out;
  json_escape(&out, in);
  VALUE rout = p2rstring(&out);
  free(out.val);
  pstring_free(in);
  return rout;
}

VALUE
rbc_isTestMode(VALUE class) {
  (void) class;
  return isTestMode() ? Qtrue : Qfalse;
}

VALUE
rbc_setTestMode(VALUE class, VALUE val) {
  (void) class;
  setTestMode(val == Qtrue);
  set_logger(stderr);
  return Qnil;
}