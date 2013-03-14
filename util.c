#include "sit.h"

bool _is_test_mode = false;

bool
isTestMode() {
  return _is_test_mode;
}

void
setTestMode(bool val) {
  _is_test_mode = val;
}

const char *json_hex_chars = "0123456789abcdefABCDEF";

#define APPEND(_s)  do { int n = sprintf(outpos, "%s", _s); outpos += n; } while(0)

void
json_escape(pstring *target, pstring *pstr) {
  char *out = malloc(pstr->len * 2);
  char *outpos = out;
  int len = pstr->len;
  const char *str = pstr->val;
  for(int i = 0; i < len; i++) {
    char c = str[i];
    if(c == '\b') APPEND("\\b");
    else if(c == '\n') APPEND("\\n");
    else if(c == '\r') APPEND("\\r");
    else if(c == '\t') APPEND("\\t");
    else if(c == '\f') APPEND("\\f");
    else if(c == '"') APPEND("\\\"");
    else if(c == '\\') APPEND("\\\\");
    else if(c == '/') APPEND("\\/");
    else if(c < ' ') {
	    int n = sprintf(outpos, "\\u00%c%c", 
	                    json_hex_chars[c >> 4],
                		  json_hex_chars[c & 0xf]);
      outpos +=n;
    } else {
      *outpos++ = c;
    }
  }
  target->len = outpos - out;
  target->val = out;
}