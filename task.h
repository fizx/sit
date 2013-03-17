#ifndef TASK_H_INCLUDED
#define TASK_H_INCLUDED

#include "engine.h"
#include "parser.h"

typedef struct Task {
  unsigned int   id;
  struct Engine *engine;
	Parser        *parser;
  void          *state;
	void          *data;

} Task;

Task *
task_new();

Task *
tail_task_new(Engine *engine, pstring *path);

void
task_free(Task *task);

#endif