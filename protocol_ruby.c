#include "sit_ruby.h"

void
_ruby_command_found(struct sit_protocol_handler *handler, pstring *command, pstring* more) {
  VALUE rcmd = p2rstring(command);
  VALUE rmore = p2rstring(more);
  VALUE block = rb_ary_entry(vunwrap(handler->data), 0);
  rb_funcall(block, rb_intern("call"), 2, rcmd, rmore);
}

void
_ruby_data_found(struct sit_protocol_handler *handler, pstring *data) {
  VALUE rdata = p2rstring(data);
  VALUE block = rb_ary_entry(vunwrap(handler->data), 1);
  rb_funcall(block, rb_intern("call"), 2, rdata);
}

void
_ruby_data_complete(struct sit_protocol_handler *handler) {
  (void) handler;
}

void
_proto_ruby_error_found(struct sit_protocol_handler *handler, pstring *message) {
  (void) handler;
  rb_raise(rb_eRuntimeError, "%.*s", message->len, message->val);
}

VALUE
rbc_protocol_new(VALUE class, VALUE rcommands, VALUE rdatas) {
	sit_protocol_parser *parser = sit_line_protocol_new();
  sit_protocol_handler *handler = parser->handler;
  SET_ONCE(handler->command_found, _ruby_command_found);
  SET_ONCE(handler->data_found, _ruby_data_found);
  SET_ONCE(handler->data_complete, _ruby_data_complete);
  SET_ONCE(handler->error_found, _proto_ruby_error_found);
  VALUE a = rb_ary_new();
  rb_ary_push(a, rcommands);
  rb_ary_push(a, rdatas);
  SET_ONCE(handler->data, vwrap(a));
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, parser);
  return tdata;
}

VALUE
rbc_line_protocol_new(VALUE class, VALUE rinput) {
  Input *input;
	Data_Get_Struct(rinput, Input, input);
	sit_protocol_parser *parser = sit_line_input_protocol_new(input);
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, parser);
  return tdata;
}

VALUE 
rbc_line_protocol_output(VALUE self) {
  sit_protocol_parser *parser;
	Data_Get_Struct(self, sit_protocol_parser, parser);
  sit_protocol_handler *handler = parser->handler;
  Input *input = handler->data;
  Output *output = input->output;
  return vunwrap(output->data);
}

VALUE 
rbc_protocol_consume(VALUE self, VALUE rstr) {
  sit_protocol_parser *protocol;
	Data_Get_Struct(self, sit_protocol_parser, protocol);
  pstring *pstr = r2pstring(rstr);
  protocol->consume(protocol, pstr);
  return Qnil;
}

VALUE
rbc_protocol_end_stream(VALUE self) {
  sit_protocol_parser *protocol;
	Data_Get_Struct(self, sit_protocol_parser, protocol);
  protocol->end_stream(protocol);
  return Qnil;
}
