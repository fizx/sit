#include "sit_ruby.h"

void 
_append_buf(struct Output *output, pstring *message) {
  VALUE rstr = p2rstring(message);
  VALUE val = vunwrap(output->data);  
  rb_ary_push(val, rstr);
}

void 
_no_op(struct Output *output) {  
  (void) output;
}

VALUE
rbc_input_new(VALUE class, VALUE rengine, VALUE rbuf_size, VALUE buf) {
	Engine *engine;
	Data_Get_Struct(rengine, Engine, engine);
	long buf_size = NUM2LONG(rbuf_size);
	Input *input = input_new(engine, buf_size);
  Output *output = malloc(sizeof(*output));
  output->data = vwrap(buf);
  output->write = _append_buf;
  output->close = _no_op;
  input->output = output;
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, input);
  return tdata;
}

VALUE 
rbc_input_output(VALUE self) {
	Input *input;
	Data_Get_Struct(self, Input, input);
  Output *output = input->output;
  return vunwrap(output->data);
}

VALUE 
rbc_input_consume(VALUE self, VALUE rstr) {
	Input *input;
	Data_Get_Struct(self, Input, input);
	pstring *pstr = r2pstring(rstr);
	input_consume(input, pstr);
	pstring_free(pstr);
	return Qnil;
}

VALUE
rbc_input_end_stream(VALUE self) {
	Input *input;
	Data_Get_Struct(self, Input, input);
	input_end_stream(input);
	return Qnil;
}