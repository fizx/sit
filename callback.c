#include "sit.h"

sit_callback *
sit_callback_new() {
	sit_callback *cb = malloc(sizeof(sit_callback));
	cb->id = -1;
	cb->next = NULL;
	cb->user_data = NULL;
	cb->free = NULL;
	return cb;
}

void
sit_callback_free(sit_callback *cb) {
	if(cb->free == NULL) {
	} else {
		cb->free(cb);
	}
}
