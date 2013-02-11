#include "util.h"
#include <stdbool.h>

bool _is_test_mode = false;

bool
isTestMode() {
  return _is_test_mode;
}

void
setTestMode(bool val) {
  _is_test_mode = val;
}