/**
 * Listing of all of the ruby classes to load.  The class definitions themselves
 * are either in Ruby inlined here (if they're really simple), or in <class>_ruby.c
 */

#include "sit_ruby.h"

VALUE rbc_pstring;
VALUE rbc_ast;
VALUE rbc_ast_node;
VALUE rbc_ring_buffer;
VALUE rbc_term;
VALUE rbc_callback;
VALUE rbc_doc_buf;
VALUE rbc_query;
VALUE rbc_conjunction;
VALUE rbc_parser;
VALUE rbc_tokenizer;
VALUE rbc_engine;
VALUE rbc_lrw_dict;
VALUE rbc_plist_pool;
VALUE rbc_plist;
VALUE rbc_vstring;
VALUE rbc_plist_cursor;
VALUE rbc_plist_entry;
VALUE rbc_result_iterator;
VALUE rbc_query_parser;
VALUE rbc_json_parser;
VALUE rbc_input;
VALUE rbc_protocol;
VALUE rbc_line_protocol;
VALUE rbc_server;
VALUE rbc_int_ring_buffer;
VALUE rbc_int_ring_buffer_cursor;
VALUE rbc_tokenizer;

void 
Init_sit() {
	VALUE m_sit = rb_define_module("Sit");
	rb_define_singleton_method(m_sit, "test_mode=", rbc_setTestMode, 1);
	rb_define_singleton_method(m_sit, "test_mode", rbc_isTestMode, 0);
	
	// PString
	rbc_pstring = rb_define_class_under(m_sit, "PString", rb_cObject);
	rb_define_singleton_method(rbc_pstring, "new", rbc_pstring_new, 1);
	rb_define_method(rbc_pstring, "to_s", rbc_pstring_to_s, 0);
	rb_define_method(rbc_pstring, "<<", rbc_pstring_append, 1);
	rb_define_method(rbc_pstring, "<=>", rbc_pstring_comparator, 1);
	rb_include_module(rbc_pstring, rb_mComparable);
	
	// AST
	rbc_ast = rb_define_class_under(m_sit, "AST", rb_cObject);
	rb_define_singleton_method(rbc_ast, "new", rbc_ast_new, 0);

	//DocBuf
  rbc_doc_buf = rb_define_class_under(m_sit, "DocBuf", rb_cObject);
	rb_define_singleton_method(rbc_doc_buf, "new", rbc_doc_buf_new, 0);
	rb_define_method(rbc_doc_buf, "field_found", rbc_doc_buf_field_found, 1);
	rb_define_method(rbc_doc_buf, "term_found", rbc_doc_buf_term_found, 2);
	rb_define_method(rbc_doc_buf, "int_found", rbc_doc_buf_int_found, 1);
	rb_define_method(rbc_doc_buf, "doc_found", rbc_doc_buf_doc_found, 1);
	rb_define_method(rbc_doc_buf, "ints", rbc_doc_buf_ints, 0);
	rb_define_method(rbc_doc_buf, "terms", rbc_doc_buf_terms, 0);
	rb_define_method(rbc_doc_buf, "doc", rbc_doc_buf_doc, 0);
  
	// AST::Node
	rbc_ast_node = rb_define_class_under(m_sit, "ASTNode", rb_cObject);
	rb_define_singleton_method(rbc_ast_node, "new", rbc_ast_node_new, 2);
	rb_define_method(rbc_ast_node, "data", rbc_ast_node_get_data, 0);
	rb_define_method(rbc_ast_node, "data=", rbc_ast_node_set_data, 1);
	rb_define_method(rbc_ast_node, "next", rbc_ast_node_next, 0);
	rb_define_method(rbc_ast_node, "prev", rbc_ast_node_prev, 0);
	rb_define_method(rbc_ast_node, "parent", rbc_ast_node_parent, 0);
	rb_define_method(rbc_ast_node, "child", rbc_ast_node_child, 0);
	rb_define_method(rbc_ast_node, "==", rbc_ast_node_equals, 1);
	rb_define_method(rbc_ast_node, "to_s", rbc_ast_node_to_s, 0);
	
	rb_define_singleton_method(rbc_ast_node, "insert_before", rbc_ast_node_insert_before, 2);
	rb_define_singleton_method(rbc_ast_node, "insert_after", rbc_ast_node_insert_after, 2);
	rb_define_singleton_method(rbc_ast_node, "prepend_child", rbc_ast_node_prepend_child, 2);
	rb_define_singleton_method(rbc_ast_node, "remove", rbc_ast_node_remove, 1);
	rb_define_singleton_method(rbc_ast_node, "wrap", rbc_ast_node_wrap, 2);
	
	// RingBuffer
	rbc_ring_buffer = rb_define_class_under(m_sit, "RingBuffer", rb_cObject);
	rb_define_singleton_method(rbc_ring_buffer, "new", rbc_ring_buffer_new, 1);
	rb_define_method(rbc_ring_buffer, "append", rbc_ring_buffer_append, 1);
	rb_define_method(rbc_ring_buffer, "get", rbc_ring_buffer_get, 2);
	
	// IntRingBuffer
	rbc_int_ring_buffer = rb_define_class_under(m_sit, "IntRingBuffer", rb_cObject);
	rb_define_singleton_method(rbc_int_ring_buffer, "new", rbc_int_ring_buffer_new, 1);
	rb_define_method(rbc_int_ring_buffer, "append", rbc_int_ring_buffer_append, 1);
	rb_define_method(rbc_int_ring_buffer, "get", rbc_int_ring_buffer_get, 1);
	
	// IntRingBufferCursor
	rbc_int_ring_buffer_cursor = rb_define_class_under(m_sit, "IntRingBufferCursor", rb_cObject);
	rb_define_singleton_method(rbc_int_ring_buffer_cursor, "new", rbc_int_ring_buffer_cursor_new, 3);
	rb_define_method(rbc_int_ring_buffer_cursor, "get", rbc_int_ring_buffer_cursor_get, 0);
	rb_define_method(rbc_int_ring_buffer_cursor, "pos", rbc_int_ring_buffer_cursor_pos, 0);
	rb_define_method(rbc_int_ring_buffer_cursor, "prev!", rbc_int_ring_buffer_cursor_prev, 0);
	
	// Term
	rbc_term = rb_define_class_under(m_sit, "Term", rb_cObject);
	rb_define_singleton_method(rbc_term, "new", rbc_term_new, 4);
	rb_define_singleton_method(rbc_term, "new_numeric", rbc_term_new_numeric, 3);
	rb_define_method(rbc_term, "to_s", rbc_term_to_s, 0);
	rb_define_method(rbc_term, "<=>", rbc_term_comparator, 1);
	rb_include_module(rbc_term, rb_mComparable);
	
	// Callback
	rbc_callback = rb_define_class_under(m_sit, "Callback", rb_cObject);
	rb_define_singleton_method(rbc_callback, "new", rbc_callback_new, 2);
	rb_define_method(rbc_callback, "call", rbc_callback_call, 0);
	rb_define_method(rbc_callback, "to_s", rbc_callback_to_s, 0);

	// Query
	rbc_query = rb_define_class_under(m_sit, "Query", rb_cObject);
	rb_define_singleton_method(rbc_query, "new", rbc_query_new, 2);
	rb_define_method(rbc_query, "to_s", rbc_query_to_s, 0);
	rb_define_method(rbc_query, "==", rbc_query_equals, 1);

	// Conjunction
	rbc_conjunction = rb_define_class_under(m_sit, "Conjunction", rb_cObject);
	rb_define_singleton_method(rbc_conjunction, "new", rbc_conjunction_new, 1);
	rb_define_method(rbc_conjunction, "to_s", rbc_conjunction_to_s, 0);
	rb_define_method(rbc_conjunction, "==", rbc_conjunction_equals, 1);

	// QueryParser
	rbc_query_parser = rb_define_class_under(m_sit, "QueryParser", rb_cObject);
	rb_define_singleton_method(rbc_query_parser, "new", rbc_query_parser_new, 0);
	rb_define_method(rbc_query_parser, "consume", rbc_query_parser_consume, 1);
	rb_define_method(rbc_query_parser, "queries", rbc_query_parser_queries, 0);
	rb_define_method(rbc_query_parser, "last_error", rbc_query_parser_last_error, 0);
	rb_define_method(rbc_query_parser, "last_ast_to_s", rbc_query_parser_last_ast_to_s, 0);
	rb_define_method(rbc_query_parser, "last_query_to_s", rbc_query_parser_last_query_to_s, 0);

	// Parser
	rbc_parser = rb_define_class_under(m_sit, "Parser", rb_cObject);
	rb_define_singleton_method(rbc_parser, "new", rbc_parser_new, 0);
	rb_define_singleton_method(rbc_parser, "new_json", rbc_parser_new_json, 0);
	rb_define_singleton_method(rbc_parser, "new_solr", rbc_parser_new_solr, 0);
	rb_define_method(rbc_parser, "consume", rbc_parser_consume, 1);
	rb_define_method(rbc_parser, "end_stream", rbc_parser_end_stream, 0);
	rb_define_method(rbc_parser, "on_document", rbc_parser_on_document, 1);
	
	// Engine
	rbc_engine = rb_define_class_under(m_sit, "Engine", rb_cObject);
	rb_define_singleton_method(rbc_engine, "new", rbc_engine_new, 2);
	rb_define_method(rbc_engine, "parser", rbc_engine_parser, 0);
	rb_define_method(rbc_engine, "register", rbc_engine_register, 1);
	rb_define_method(rbc_engine, "unregister", rbc_engine_unregister, 1);
	rb_define_method(rbc_engine, "queries", rbc_engine_queries, 0);
	rb_define_method(rbc_engine, "last_document", rbc_engine_last_document, 0);
	rb_define_method(rbc_engine, "last_document_id", rbc_engine_last_document_id, 0);
	rb_define_method(rbc_engine, "search", rbc_engine_search, 1);
	rb_define_method(rbc_engine, "get_int", rbc_engine_get_int, 2);
	rb_define_method(rbc_engine, "incr", rbc_engine_incr, 3);
	rb_define_method(rbc_engine, "set_int", rbc_engine_set_int, 3);

	// LrwDict
	rbc_lrw_dict = rb_define_class_under(m_sit, "LrwDict", rb_cObject);
	rb_define_singleton_method(rbc_lrw_dict, "new", rbc_lrw_dict_new, 1);
	rb_define_method(rbc_lrw_dict, "[]", rbc_lrw_dict_get, 1);
	rb_define_method(rbc_lrw_dict, "[]=", rbc_lrw_dict_put, 2);
	rb_define_method(rbc_lrw_dict, "capacity", rbc_lrw_dict_capacity, 0);
	rb_define_method(rbc_lrw_dict, "size", rbc_lrw_dict_size, 0);
	rb_define_method(rbc_lrw_dict, "each", rbc_lrw_dict_each, 0);
	rb_include_module(rbc_lrw_dict, rb_mEnumerable);
	
	// PlistPool
	rbc_plist_pool = rb_define_class_under(m_sit, "PlistPool", rb_cObject);
	rb_define_singleton_method(rbc_plist_pool, "new", rbc_plist_pool_new, 1);
	rb_define_method(rbc_plist_pool, "capacity", rbc_plist_pool_capacity, 0);

	// Plist
	rbc_plist = rb_define_class_under(m_sit, "Plist", rb_cObject);
	rb_define_singleton_method(rbc_plist, "new", rbc_plist_new, 1);
	rb_define_method(rbc_plist, "size", rbc_plist_size, 0);
	rb_define_method(rbc_plist, "blocks_count", rbc_plist_blocks_count, 0);
	rb_define_method(rbc_plist, "region", rbc_plist_region, 0);
	rb_define_method(rbc_plist, "append", rbc_plist_append, 1);
	rb_define_method(rbc_plist, "each", rbc_plist_each, 0);
	rb_define_method(rbc_plist, "ptr", rbc_plist_ptr, 0);
	rb_define_method(rbc_plist, "free", rbc_plist_free, 0);
	rb_define_method(rbc_plist, "new_cursor", rbc_plist_new_cursor, 0);
	rb_include_module(rbc_plist, rb_mEnumerable);
	
	// PlistCursor
	rbc_plist_cursor = rb_define_class_under(m_sit, "PlistCursor", rb_cObject);
	rb_define_method(rbc_plist_cursor, "prev!", rbc_plist_cursor_prev, 0);
	rb_define_method(rbc_plist_cursor, "next!", rbc_plist_cursor_next, 0);
	rb_define_method(rbc_plist_cursor, "entry", rbc_plist_cursor_entry, 0);
	
	// Server
	rbc_server = rb_define_class_under(m_sit, "Server", rb_cObject);
	rb_define_singleton_method(rbc_server, "new", rbc_server_new, 1);
	rb_define_method(rbc_server, "start", rbc_server_start, 1);

	// PlistEntry
	rbc_plist_entry = rb_define_class_under(m_sit, "PlistEntry", rb_cObject);
	rb_define_singleton_method(rbc_plist_entry, "new", rbc_plist_entry_new, 2);
	rb_define_method(rbc_plist_entry, "to_s", rbc_plist_entry_to_s, 0);
	rb_define_method(rbc_plist_entry, "==", rbc_plist_entry_equals, 1);
	
	// ResultIterator
	rbc_result_iterator = rb_define_class_under(m_sit, "ResultIterator", rb_cObject);
	rb_define_method(rbc_result_iterator, "prev!", rbc_result_iterator_prev, 0);
	rb_define_method(rbc_result_iterator, "document", rbc_result_iterator_document, 0);
	rb_define_method(rbc_result_iterator, "call", rbc_result_iterator_call, 0);
	rb_define_method(rbc_result_iterator, "document_id", rbc_result_iterator_document_id, 0);	
	
	// Input
	rbc_input = rb_define_class_under(m_sit, "Input", rb_cObject);
	rb_define_singleton_method(rbc_input, "new", rbc_input_new, 3);
	rb_define_method(rbc_input, "consume", rbc_input_consume, 1);
	rb_define_method(rbc_input, "end_stream", rbc_input_end_stream, 0);
	
	// VString
	rbc_vstring = rb_define_class_under(m_sit, "VString", rb_cObject);
	rb_define_singleton_method(rbc_vstring, "new", rbc_vstring_new, 0);
	rb_define_method(rbc_vstring, "append", rbc_vstring_append, 1);
	rb_define_method(rbc_vstring, "get", rbc_vstring_get, 2);
	rb_define_method(rbc_vstring, "gets", rbc_vstring_gets, 0);
	rb_define_method(rbc_vstring, "shift", rbc_vstring_shift, 1);
	rb_define_method(rbc_vstring, "to_s", rbc_vstring_to_s, 0);
	rb_define_method(rbc_vstring, "size", rbc_vstring_size, 0);
	rb_define_method(rbc_vstring, "nodes", rbc_vstring_nodes, 0);

	// Protocol
  rbc_protocol = rb_define_class_under(m_sit, "Protocol", rb_cObject);
  rb_define_singleton_method(rbc_protocol, "new", rbc_protocol_new, 2);
  rb_define_method(rbc_protocol, "consume", rbc_protocol_consume, 1);
  rb_define_method(rbc_protocol, "end_stream", rbc_protocol_end_stream, 0);
  
  rbc_line_protocol = rb_define_class_under(m_sit, "LineProtocol", rb_cObject);
  rb_define_singleton_method(rbc_line_protocol, "new", rbc_line_protocol_new, 1);
  rb_define_method(rbc_line_protocol, "consume", rbc_protocol_consume, 1);
  rb_define_method(rbc_line_protocol, "end_stream", rbc_protocol_end_stream, 0);

  rbc_tokenizer = rb_define_class_under(m_sit, "Tokenizer", rb_cObject);
  rb_define_singleton_method(rbc_tokenizer, "new_whitespace", rbc_tokenizer_new_whitespace, 1);
  rb_define_method(rbc_tokenizer, "consume", rbc_tokenizer_consume, 1);
}
