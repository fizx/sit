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
	void         (*tell)(struct Task *task, pstring *message);
  pstring     *(*to_json)(struct Task *task);
  void         (*free)(struct Task *task);
} Task;

Task *
task_new();

pstring *
task_to_json(Task *task);

Task *
client_task_new(struct Engine *engine, pstring *hostport);

long
task_last_id();

Task *
tail_task_new(struct Engine *engine, pstring *path, double interval);

void
task_free(Task *task);

#endif