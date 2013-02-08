#ifndef DICT_TYPES_H_INCLUDED
#define DICT_TYPES_H_INCLUDED

#include "pstring.h"
#include "dict.h"
#include "lrw_dict.h"

dictType *
getPstrDict();

lrw_type *
getTermLrw(); 

dictType *
getTermDict();

#endif