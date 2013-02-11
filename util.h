#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <assert.h>
#include <stdbool.h>
#include "sit.h"

#define SET_ONCE(variable, value) ((variable && variable != value) ? assert(0) : (void)(variable = value))

bool
isTestMode();

void
setTestMode(bool val);

#endif
