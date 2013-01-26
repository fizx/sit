#include <stdlib.h>
#include "pstring.h"

pstring *
pstring_new(int len) {
	pstring *pstr = malloc(sizeof(pstring));
	pstr->val = malloc(len);
	pstr->len = len;
	return pstr;
}

void
pstring_free(pstring *pstr) {
	if(pstr != NULL) {
		free(pstr->val);
		free(pstr);
	}
}