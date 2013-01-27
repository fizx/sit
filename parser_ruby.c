#include "parser_ruby.h"
#include "pstring.h"
#include "pstring_ruby.h"
#include "assert.h"
#include "sit_engine.h"
#include "sit_parser.h"

void 
rbc_consume(void *engine, pstring *str) {
	assert(0);
}

VALUE 
rbc_parser_new(VALUE class, VALUE rengine) {
	sit_engine *engine;
	Data_Get_Struct(rengine, sit_engine, engine);
	
	sit_parser *parser = sit_parser_new();
	VALUE tdata = Data_Wrap_Struct(class, NULL, NULL, parser);
	parser->data = engine;
	parser->consume = rbc_consume;
	return tdata;
}

VALUE
rbc_parser_consume(VALUE self, VALUE rstr) {
	sit_parser *parser;
	Data_Get_Struct(self, sit_parser, parser);
	sit_engine *engine = parser->data;
	pstring *pstr = r2pstring(rstr);
	sit_engine_append(engine, pstr);
	pstring_free(pstr);
	return Qnil;
}

VALUE
rbc_parser_term_found(VALUE self, VALUE roff, VALUE rlen, VALUE rfield_offset){ 
	sit_parser *parser;
	Data_Get_Struct(self, sit_parser, parser);
	sit_engine *engine = parser->data;
	long off = NUM2LONG(roff);
	int len = NUM2INT(rlen);
	int field_offset = NUM2INT(rfield_offset);
	sit_engine_term_found(engine, off, len, field_offset);
	return Qnil;
}

VALUE
rbc_parser_document_found(VALUE self, VALUE roff, VALUE rlen) {
	sit_parser *parser;
	Data_Get_Struct(self, sit_parser, parser);
	sit_engine *engine = parser->data;
	long off = NUM2LONG(roff);
	int len = NUM2INT(rlen);
	sit_engine_document_found(engine, off, len);
	return Qnil;
}

VALUE 
rbc_parser_field_found(VALUE self, VALUE rstr) {
	sit_parser *parser;
	Data_Get_Struct(self, sit_parser, parser);
	sit_engine *engine = parser->data;
	pstring *pstr = r2pstring(rstr);
	sit_engine_field_found(engine, pstr);
	return Qnil;
}
