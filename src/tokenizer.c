#include "sit.h"

Tokenizer *
tokenizer_new() {
  return calloc(1, sizeof(Tokenizer));
}

void
tokenizer_free(Tokenizer *tok) {
  if(tok->free) {
    tok->free(tok);
  } else {
    if(tok->on_error) callback_free(tok->on_error);
    if(tok->on_token) callback_free(tok->on_token);
    free(tok);
  }
}