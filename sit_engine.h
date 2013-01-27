#ifndef SIT_ENGINE_H_INCLUDED
#define SIT_ENGINE_H_INCLUDED

#include "pstring.h"

typedef struct {
	
} sit_engine;

void 
sit_engine_term_found(void *engine, long off, int len, int field_offset);

void 
sit_engine_document_found(void *engine, long off, int len);

void 
sit_engine_field_found(void *engine, pstring *name);

void
sit_engine_append(void *engine, pstring *pstr);

#endif

