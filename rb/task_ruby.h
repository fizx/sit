#ifndef TASK_RUBY_H_INCLUDED
#define TASK_RUBY_H_INCLUDED

#include "ruby.h"

VALUE 
rbc_task_new_tail(VALUE class, VALUE engine, VALUE path);

VALUE 
rbc_task_to_json(VALUE self);

VALUE
rbc_task_id(VALUE self);

#endif
