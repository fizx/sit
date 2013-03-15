#ifndef REGEX_TOKENIZER_H_INCLUDED
#define REGEX_TOKENIZER_H_INCLUDED

#include "tokenizer.h"

Tokenizer *
regex_tokenizer_new(char *regex);

Tokenizer *
regex_fresh_copy(Tokenizer *tok);

#endif
