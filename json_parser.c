#include "json_parser.h"
#include "jsonsl.h"
#include "white_parser.h"
#include "white_parser.h"

typedef struct {
  jsonsl_t    json_parser;
  sit_parser *tokenizer;
  pstring    *active;
  long        active_off;
  long        start_off;
  long        doc_off;
  pstring    *buf;
  int         token;
  bool        buffering;
} json_state;

int 
_jsonsl_error_callback(
        jsonsl_t jsn,
        jsonsl_error_t error,
        struct jsonsl_state_st* state,
        jsonsl_char_t *at) {
  (void) state;
  (void) at;
	sit_parser *parser = jsn->data;
  parser->error_found(parser, c2pstring(jsonsl_strerror(error)));
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
	sit_parser *parser = jsn->data;
	json_state *sit_state = parser->state;
	switch (action) {
	case JSONSL_ACTION_PUSH: 
		switch (state->type) {
		case JSONSL_T_HKEY:
		case JSONSL_T_STRING:	  
      sit_state->buffering = true;                                            
      off = at + 1 - sit_state->active->val;   
      sit_state->start_off = sit_state->active_off + off;
      len = sit_state->active->len - off;      
      sit_state->buf = pstring_new2(at + 1, len);
			break;
		}  
		case JSONSL_T_OBJECT:
		  if (*at == '{') {
  		  off = at - sit_state->active->val;
  		  sit_state->doc_off = sit_state->active_off + off;
      }
		break;
	case JSONSL_ACTION_POP: 
		switch (state->type) {
		case JSONSL_T_HKEY:
      len = state->pos_cur - state->pos_begin - 1;
      sit_state->buf->len = len;
      parser->field_found(parser, sit_state->buf);
      sit_state->buffering = false;
			break;
		case JSONSL_T_STRING:
      len = state->pos_cur - state->pos_begin - 1;
      sit_state->buf->len = len;
      sit_state->tokenizer->term_found = parser->term_found;
      sit_state->tokenizer->data = parser->data;
      sit_state->tokenizer->set_offset(sit_state->tokenizer, sit_state->start_off);
      sit_state->tokenizer->consume(sit_state->tokenizer, sit_state->buf);
      sit_state->tokenizer->end_stream(sit_state->tokenizer);
      sit_state->buffering = false;
			break;
		case JSONSL_T_OBJECT:  
      off = at + 1 - sit_state->active->val;
      end = sit_state->active_off + off;
		  parser->document_found(parser, sit_state->doc_off, off - sit_state->doc_off);
		  jsonsl_reset(sit_state->json_parser);
      break;
		}
		break;
	case JSONSL_ACTION_ERROR: 
		break;
	}
}

sit_parser *
json_white_parser_new(sit_engine *engine) {
  return json_parser_new(engine, white_parser_new());
}

void 
_json_consume(struct sit_parser *parser, pstring *str) {
  json_state *state = parser->state;
  state->active = str;
  if(state->buffering) {
    padd(state->buf, str);
  }
	jsonsl_feed(state->json_parser, str->val, str->len);
  state->active_off += str->len;
}

sit_parser *
json_parser_new(sit_engine *engine, sit_parser *tokenizer) {
  sit_parser *parser = sit_parser_new(engine);
  parser->state = malloc(sizeof(json_state));
  json_state *state = parser->state;
  state->json_parser = jsonsl_new(100);
  jsonsl_enable_all_callbacks(state->json_parser);
  jsonsl_reset(state->json_parser);
  state->tokenizer = tokenizer;
  state->active_off = 0;
  state->token = 0;
  state->json_parser->action_callback = _jsonsl_stack_callback;
	state->json_parser->error_callback = _jsonsl_error_callback;
	state->json_parser->data = parser;
	
  parser->consume = _json_consume;
  return parser;
}

