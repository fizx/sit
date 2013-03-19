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
  pstring     *(*to_json)(struct Task *task);
  void         (*free)(struct Task *task);
} Task;

Task *
task_new();

pstring *
task_to_json(Task *task);

Task *
client_task_new(Engine *engine, pstring *hostport);

Task *
tail_task_new(Engine *engine, pstring *path, double interval);

void
task_free(Task *task);

#endif