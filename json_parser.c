#include "sit.h"

#define MAX_DEPTH 10

typedef struct JSONState {
  jsonsl_t    json_parser;
  Tokenizer  *tokenizer;
  long        begins[MAX_DEPTH];
  DocBuf     *buffers[MAX_DEPTH];
  int level;
  pstring    *working;      // the live window we care about
  long        working_pos;  // the offset of working relative to the stream start
  pstring    *delta;        // the string passed with consume
} JSONState;

void
_json_token_found(Callback *cb, void *data) {
  Token *token = data;
  JSONState *state = cb->user_data;
  doc_buf_term_found(state->buffers[state->level], token->text, token->offset);
}

pstring LEVEL = {
  "_level",
  6
};

int 
_jsonsl_error_callback(
        jsonsl_t jsn,
        jsonsl_error_t error,
        struct jsonsl_state_st* state,
        jsonsl_char_t *at) {
  (void) state;
  (void) at;
	Parser *parser = jsn->data;
  parser->on_error->handler(parser->on_error, c2pstring(jsonsl_strerror(error)));
	return 0;
}

void 
_jsonsl_stack_callback(
        jsonsl_t jsn,
        jsonsl_action_t action,
        struct jsonsl_state_st* state,
        const jsonsl_char_t *at) {
	Parser *parser = jsn->data;
	JSONState *mystate = parser->state;
  pstring *working = mystate->working;
  long working_pos = mystate->working_pos;
  mystate->level = state->level;
	switch (action) {
	case JSONSL_ACTION_PUSH:
	  switch (state->type) {
		case JSONSL_T_OBJECT: {
      DEBUG("level: %d", state->level);
      DEBUG("start: %d", jsn->pos);
      mystate->begins[state->level] = jsn->pos;
      break;
		}}
    break;
	case JSONSL_ACTION_POP: 
		switch (state->type) {
	  case JSONSL_T_SPECIAL: {
  	  if(!(state->special_flags & JSONSL_SPECIALf_NUMERIC)) {
        break;
      }
      const char *string = working->val + state->pos_begin - working_pos;
      DEBUG("level: %d", state->level);
      DocBuf *buf = mystate->buffers[state->level-1];
      doc_buf_int_found(buf, strtol(string, NULL, 10));
      break;
    }
		case JSONSL_T_HKEY: {
      long off = state->pos_begin - working_pos + 1;
      int len = state->pos_cur - state->pos_begin - 1;
      pstring pstr = {
        working->val + off,
        len
      };
      DEBUG("level: %d", state->level);
      DocBuf *buf = mystate->buffers[state->level-1];
      doc_buf_field_found(buf, &pstr);
			break;
		}
		case JSONSL_T_STRING: {
  		long off = state->pos_begin - working_pos + 1;
      int len = state->pos_cur - state->pos_begin - 1;
      pstring pstr = {
        working->val + off,
        len
      };

      DEBUG("level: %d", state->level);
      mystate->level = state->level - 1;
      mystate->tokenizer->data = parser->data;
      mystate->tokenizer->consume(mystate->tokenizer, &pstr);

			break;
    }
		case JSONSL_T_OBJECT: {
     	DocBuf *buf = mystate->buffers[state->level];
      long off = mystate->begins[state->level] - working_pos;
      long len = jsn->pos - mystate->begins[state->level] + 1;
      pstring doc = {
        working->val + off,
        len
      };
      DEBUG("level: %d", state->level);
      doc_buf_field_found(buf, &LEVEL);
      doc_buf_int_found(buf, state->level);
		  doc_buf_doc_found(buf, &doc);
		  parser->on_document->handler(parser->on_document, buf);
      doc_buf_reset(buf);
		  if(state->level == 1) {
        pstring *tmp = working;
        mystate->working = pstring_new2(working->val + jsn->pos, working->len - jsn->pos);
        pstring_free(tmp);
        jsonsl_reset(jsn);
        mystate->working_pos = 0;
		  }
      break;
		}}
		break;
	case JSONSL_ACTION_ERROR: 
		break;
	}
}

Parser *
json_white_parser_new() {
  return json_parser_new(white_tokenizer_new());
}

Parser *
json_fresh_copy(Parser *parser) {
	JSONState *state = parser->state;
	return json_parser_new(state->tokenizer->fresh_copy(state->tokenizer));
}

void 
_json_consume(struct Parser *parser, pstring *str) {
  JSONState *state = parser->state;
  padd(state->working, str);
  state->delta = str;
	jsonsl_feed(state->json_parser, str->val, str->len);
}

void 
_json_parser_free(void *data) {
  Parser *parser = data;
  JSONState *state = parser->state;
  jsonsl_destroy(state->json_parser);
  tokenizer_free(state->tokenizer);
  pstring_free(state->working);
  for(int i =0; i < MAX_DEPTH; i++) {
    free(state->buffers[i]);
  }
  free(state);
  free(parser);
}

Parser *
json_parser_new(Tokenizer *tokenizer) {
  Parser *parser = parser_new();
  parser->state = calloc(1, sizeof(JSONState));
  JSONState *state = parser->state;
  parser->free = _json_parser_free;
  state->json_parser = jsonsl_new(MAX_DEPTH);
  jsonsl_enable_all_callbacks(state->json_parser);
  jsonsl_reset(state->json_parser);
  state->tokenizer = tokenizer;
  state->tokenizer->on_token = callback_new(_json_token_found, state);
  state->json_parser->action_callback = _jsonsl_stack_callback;
	state->json_parser->error_callback = _jsonsl_error_callback;
	SET_ONCE(state->json_parser->data, parser);
	parser->fresh_copy = json_fresh_copy;
  parser->consume = _json_consume;
  state->working = pstring_new(0);
  state->working_pos = 0;
  for(int i =0; i < MAX_DEPTH; i++) {
    state->buffers[i] = doc_buf_new(10000);
  }
  return parser;
}
