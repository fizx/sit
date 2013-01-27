#include "sit_callback.h"
#include <stdlib.h>

static int counter = 0;

sit_callback *
sit_callback_new() {
	sit_callback *cb = malloc(sizeof(sit_callback));
	cb->id = counter++;
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
