#ifndef DICT_TYPES_H_INCLUDED
#define DICT_TYPES_H_INCLUDED

#include "dict.h"

dictType *
getPstrDict();

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

#endif