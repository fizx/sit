#include "sit.h"

#define MAX_DEPTH 10

typedef struct JSONState {
  jsonsl_t    json_parser;
  Tokenizer  *tokenizer;
  long        begins[MAX_DEPTH];
  DocBuf     *buffers[MAX_DEPTH];
  int level;
  vstring    *stream;      // the live window we care about
  int last_field_idx;
} JSONState;

void
_json_token_found(Callback *cb, void *data) {
  Token *token = data;
  JSONState *state = cb->user_data;
  DEBUG("raw term_found level: %d", state->level);
  assert(state->last_field_idx == state->level);
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
  mystate->level = state->level;
  vstring *stream = mystate->stream;
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
      pstring pstr = { NULL, state->pos_cur - state->pos_begin };
      vstring_get(&pstr, stream, state->pos_begin);
      DEBUG("level: %d", state->level);
      DocBuf *buf = mystate->buffers[state->level-1];
      if(mystate->last_field_idx == state->level - 1) {
        doc_buf_int_found(buf, strtol(pstr.val, NULL, 10));
      } else if (mystate->last_field_idx < state->level - 1) {
        WARN("int too deep");
      } else {
        ERR("wtf int too shallow");
      }
      break;
    }
   case JSONSL_T_HKEY: {
     printf("Z: %d\n", state->pos_cur);
     if(state->pos_cur > 610) {
       puts("hai");
     }
     
     int len = state->pos_cur - state->pos_begin -1;
      pstring pstr = { NULL, len };
      vstring_get(&pstr, stream, state->pos_begin+1);
      DEBUG("level: %d", state->level);
      DEBUG("raw field_found %d level: %d", pstr.val ,state->level-1);
      mystate->last_field_idx = state->level - 1;
      doc_buf_field_found(mystate->buffers[state->level-1], &pstr);
     break;
   }
   case JSONSL_T_STRING: {
     int len = state->pos_cur - state->pos_begin - 1;
     pstring pstr = { NULL, len};
     vstring_get(&pstr, stream, state->pos_begin + 1);

      DEBUG("level: %d", state->level);
      mystate->level = state->level - 1;
      mystate->tokenizer->data = parser->data;
      mystate->tokenizer->consume(mystate->tokenizer, &pstr);

     break;
    }
   case JSONSL_T_OBJECT: {
       DocBuf *buf = mystate->buffers[state->level];
      long off = mystate->begins[state->level];
      long len = jsn->pos - mystate->begins[state->level] + 1;
      pstring pstr = {NULL, len};
      vstring_get(&pstr, stream, state->pos_begin);
      DEBUG("level: %d", state->level);
      doc_buf_field_found(buf, &LEVEL);
      doc_buf_int_found(buf, state->level);
     doc_buf_doc_found(buf, &pstr);
     parser->on_document->handler(parser->on_document, buf);
      doc_buf_reset(buf);
     if(state->level == 1) {
       vstring_shift(stream, jsn->pos);
       jsonsl_reset(jsn);
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
  vstring_append(state->stream, str);
  assert(str->val != state->stream->node->pstr.val);
  jsonsl_feed(state->json_parser, state->stream->node->pstr.val, state->stream->node->pstr.len);
}

void 
_json_parser_free(void *data) {
  Parser *parser = data;
  JSONState *state = parser->state;
  jsonsl_destroy(state->json_parser);
  tokenizer_free(state->tokenizer);
  vstring_free(state->stream);
  for(int i =0; i < MAX_DEPTH; i++) {
    doc_buf_free(state->buffers[i]);
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
  state->stream = vstring_new();
  for(int i =0; i < MAX_DEPTH; i++) {
    state->buffers[i] = doc_buf_new(10000);
  }
  return parser;
}
