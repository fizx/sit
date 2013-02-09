#include "sit_input_ruby.h"
#include "sit_ruby.h"
#include "sit.h"
#include <assert.h>

VALUE
rbc_input_new(VALUE class, VALUE rengine, VALUE rterm_capacity, VALUE rbuf_size) {
	sit_engine *engine;
	Data_Get_Struct(rengine, sit_engine, engine);
	long term_capacity = NUM2LONG(rterm_capacity);
	long buf_size = NUM2LONG(rbuf_size);
	sit_input *input = sit_input_new(engine, term_capacity, buf_size);
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, input);
  return tdata;
}

VALUE 
rbc_input_consume(VALUE self, VALUE rstr) {
	sit_input *input;
	Data_Get_Struct(self, sit_input, input);
	pstring *pstr = r2pstring(rstr);
  assert(input->parser->receiver == &input->as_receiver);
	sit_input_consume(input, pstr);
	pstring_free(pstr);
	return Qnil;
}

VALUE
rbc_input_end_stream(VALUE self) {
	sit_input *input;
	Data_Get_Struct(self, sit_input, input);
	sit_input_end_stream(input);
	return Qnil;
}