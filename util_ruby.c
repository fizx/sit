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

static int loops = 0;

void
_ev_loop_until(struct ev_loop *loop, struct ev_timer *timer, int revents) {
  if (loops++ > 1000) {
		rb_raise(rb_eRuntimeError, "loop timeout");
    ev_break (EV_A_ EVBREAK_ONE);
  } else {
    if(rb_yield(rb_ary_new()) == Qtrue){
      ev_break (EV_A_ EVBREAK_ONE);
    }
  }
}

VALUE 
rbc_ev_loop_until(VALUE class) {
  struct ev_loop *loop = EV_DEFAULT;
  ev_timer *timer = malloc(sizeof(ev_timer));
  loops = 0;
  ev_timer_init(timer, _ev_loop_until, 0.01, 0.01);
  ev_timer_start(loop, timer);
  ev_run(loop, 0);
  return Qnil;
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