#include "sit.h"

typedef struct JSONState {
  jsonsl_t    json_parser;
  Parser     *tokenizer;
  pstring    *active;
  long        active_off;
  long        start_off;
  long        doc_off;
  pstring    *buf;
  int         token;
  bool        buffering;
} JSONState;

int 
_jsonsl_error_callback(
        jsonsl_t jsn,
        jsonsl_error_t error,
        struct jsonsl_state_st* state,
        jsonsl_char_t *at) {
  (void) state;
  (void) at;
	Parser *parser = jsn->data;
  parser->receiver->error_found(parser->receiver, c2pstring(jsonsl_strerror(error)));
	return 0;
}

void 
_jsonsl_stack_callback(
        jsonsl_t jsn,
        jsonsl_action_t action,
        struct jsonsl_state_st* state,
        const jsonsl_char_t *at) {
  int off;
  int len;
  int end;
  char *ptr = at;
	Parser *parser = jsn->data;
	JSONState *mystate = parser->state;
	switch (action) {
	case JSONSL_ACTION_PUSH: 
		switch (state->type) {
	  case JSONSL_T_SPECIAL:
	    if(!(state->special_flags & JSONSL_SPECIALf_NUMERIC)) {
        break;
      }
      ptr--; //no opening quote
		case JSONSL_T_HKEY:
		case JSONSL_T_STRING:	  
      mystate->buffering = true;                                            
      off = ptr + 1 - mystate->active->val;   
      mystate->start_off = mystate->active_off + off;
      len = mystate->active->len - off;      
      mystate->buf = pstring_new2(ptr + 1, len);
			break;
		}  
		case JSONSL_T_OBJECT:
		  if (*at == '{') {
  		  off = at - mystate->active->val;
  		  mystate->doc_off = mystate->active_off + off;
      }
		break;
	case JSONSL_ACTION_POP: 
		switch (state->type) {
	  case JSONSL_T_SPECIAL:  
  	  if(!(state->special_flags & JSONSL_SPECIALf_NUMERIC)) {
        break;
      }
      len = state->pos_cur - state->pos_begin;
      mystate->buf->len = len;
      parser->receiver->int_found(parser->receiver, strtol(mystate->buf->val, NULL, 10));
      break;
		case JSONSL_T_HKEY:
      len = state->pos_cur - state->pos_begin - 1;
      mystate->buf->len = len;
      parser->receiver->field_found(parser->receiver, mystate->buf);
      mystate->buffering = false;
			break;
		case JSONSL_T_STRING:
      len = state->pos_cur - state->pos_begin - 1;
      mystate->buf->len = len;
      mystate->tokenizer->data = parser->data;
			mystate->tokenizer->receiver = parser->receiver;
      mystate->tokenizer->set_offset(mystate->tokenizer, mystate->start_off);
      mystate->tokenizer->consume(mystate->tokenizer, mystate->buf);
      mystate->tokenizer->end_stream(mystate->tokenizer);
      mystate->buffering = false;
			break;
		case JSONSL_T_OBJECT:  
      off = at + 1 - mystate->active->val;
      end = mystate->active_off + off;
		  parser->receiver->document_found(parser->receiver, mystate->doc_off, end - mystate->doc_off);
		  jsonsl_reset(mystate->json_parser);
      break;
		}
		break;
	case JSONSL_ACTION_ERROR: 
		break;
	}
}

Parser *
json_white_parser_new() {
  return json_parser_new(white_parser_new());
}

Parser *
json_fresh_copy(Parser *parser) {
	JSONState *state = parser->state;
	return json_parser_new(state->tokenizer->fresh_copy(state->tokenizer));
}

void 
_json_consume(struct Parser *parser, pstring *str) {
  assert(parser->receiver);
  JSONState *state = parser->state;
  state->active = str;
  if(state->buffering) {
    padd(state->buf, str);
  }
	jsonsl_feed(state->json_parser, str->val, str->len);
  state->active_off += str->len;
}

Parser *
json_parser_new(Parser *tokenizer) {
  Parser *parser = parser_new();
  parser->state = malloc(sizeof(JSONState));
  JSONState *state = parser->state;
  state->json_parser = jsonsl_new(100);
  jsonsl_enable_all_callbacks(state->json_parser);
  jsonsl_reset(state->json_parser);
  state->tokenizer = tokenizer;
  state->active_off = 0;
  state->token = 0;
	state->buf = pstring_new(0);
  state->json_parser->action_callback = _jsonsl_stack_callback;
	state->json_parser->error_callback = _jsonsl_error_callback;
	SET_ONCE(state->json_parser->data, parser);
	parser->fresh_copy = json_fresh_copy;
  parser->consume = _json_consume;
  return parser;
}

