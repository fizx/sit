#include "sit_engine.h"
#include <stdlib.h>
#include <stdio.h>

sit_engine *
sit_engine_new(long size) {
	printf("Faux making engine of size: %ld\n", size);
	return malloc(sizeof(sit_engine));
}