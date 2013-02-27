#include "sit.h"

Callback *
callback_new() {
	Callback *cb = malloc(sizeof(Callback));
	cb->id = -1;
	cb->next = NULL;
	cb->user_data = NULL;
	cb->free = NULL;
	return cb;
}

void
callback_free(Callback *cb) {
	if(cb->free == NULL) {
	} else {
		cb->free(cb);
	}
}
