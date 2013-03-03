#ifndef DICT_TYPES_H_INCLUDED
#define DICT_TYPES_H_INCLUDED

#include "dict.h"
#include "lrw_dict.h"

dictType *
getPstrDict();

struct lrw_type *
getTermLrw(); 

dictType *
getTermDict();

dictType *
getTermPlistDict();

dictType *
getTermTermDict();

dictType *
getPstrRingBufferDict();

#endif