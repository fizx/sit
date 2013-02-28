#include "sit.h"

Callback *
callback_new(void (*handler)(struct Callback *self, void *data), void *user_data) {
	Callback *cb = malloc(sizeof(Callback));
	cb->id = -1;
	cb->next = NULL;
  cb->user_data = user_data;
  cb->handler = handler;
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
