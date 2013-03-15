#include "sit.h"

typedef struct RegexTokenizerState {
  char *pattern;
  pcre *regex;
  pcre_extra *extra;
  char *error;
  int   error_offset;
} RegexTokenizerState;

void
_regex_consume(Tokenizer *tok, pstring *str) {
  RegexTokenizerState *state = tok->state;
  int len = str->len;
  int ovector[10];
  int out = 0;
  int off = 0;
  int ahead = 0;
  pstring text;
  Token t = { &text, 0 };
  while(off < len) {
    int out = pcre_exec(state->regex, state->extra, str->val, str->len, off + ahead, 0, ovector, 10);
    int start = len;
    int end = len;
    if(out >= 0) {
      start = ovector[0];
      end = ovector[1];
    }
    if(start != off) {
      text.val = str->val + off;
      text.len = start - off;
      tok->on_token->handler(tok->on_token, &t);
      t.offset++;
      ahead = 0;
    } else {
      ahead++;
    }
    off = end;
  }
}

Tokenizer *
regex_fresh_copy(Tokenizer *t) {
	(void) t;
  RegexTokenizerState *state = t->state;
	return regex_tokenizer_new(state->pattern);
}

void
_regex_tokenizer_free(void *data) {
  Tokenizer *tok = data;
  RegexTokenizerState *state = tok->state;
  pcre_free_study(state->extra);
  pcre_free(state->regex);
  free(tok);
}

Tokenizer *
regex_tokenizer_new(char *pattern) {
  Tokenizer *tok = tokenizer_new();
  tok->consume = _regex_consume;
	tok->fresh_copy = regex_fresh_copy;
  tok->free = _regex_tokenizer_free;
  RegexTokenizerState *state = calloc(1, sizeof(RegexTokenizerState));
  tok->state = state;
  state->pattern = pattern;
  state->regex = pcre_compile(pattern, 0, &state->error, &state->error_offset, NULL);
  state->extra = pcre_study(state->regex, 0, &state->error);
  if(state->error) {
    ERR("%s", state->error);
    abort();
  }
  return tok;
}