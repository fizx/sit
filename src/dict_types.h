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
getTermQueryNodeDict();

dictType *
getTermPlistDict();

dictType *
getTermTermDict();

dictType *
getPstrRingBufferDict();

dictType *
getTermPlistCursorDict();

dictType *
getPstrParserDict();

dictType *
getDocRefDict();

dictType *
getTaskTaskDict();

#endif