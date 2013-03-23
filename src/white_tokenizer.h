#ifndef WHITE_TOKENIZER_H_INCLUDED
#define WHITE_TOKENIZER_H_INCLUDED

#include "tokenizer.h"

Tokenizer *
white_tokenizer_new();

Tokenizer *
white_fresh_copy(Tokenizer *tok);

#endif
