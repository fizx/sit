#include "sit_ruby.h"
#include "sit.h"

void
_ruby_command_found(struct sit_protocol_handler *handler, pstring *command, int argc, pstring** argv) {
  VALUE rcmd = p2rstring(command);
  VALUE ary = rb_ary_new();
  for(int i = 0; i < argc; i++) {
    rb_ary_push(ary, p2rstring(argv[i]));
  }
  VALUE block = rb_ary_entry(vunwrap(handler->data), 0);
  rb_funcall(block, rb_intern("call"), 2, rcmd, ary);
}

void
_ruby_data_found(struct sit_protocol_handler *handler, pstring *data) {
  VALUE rdata = p2rstring(data);
  VALUE block = rb_ary_entry(vunwrap(handler->data), 1);
  rb_funcall(block, rb_intern("call"), 2, rdata);
}

void
_ruby_data_complete(struct sit_protocol_handler *handler) {
}

void
_proto_ruby_error_found(struct sit_protocol_handler *handler, pstring *message) {
  rb_raise(rb_eRuntimeError, p2rstring(message));
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
