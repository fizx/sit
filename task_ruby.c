#include "sit_ruby.h"

VALUE 
rbc_task_new_tail(VALUE class, VALUE rengine, VALUE rpath) {
  Engine *engine;
	Data_Get_Struct(rengine, Engine, engine);
	
  pstring *path = r2pstring(rpath);
  Task *task = tail_task_new(engine, path, 0.01);
  pstring_free(path);
	VALUE tdata = Data_Wrap_Struct(class, NULL, task_free, task);
	rb_obj_call_init(tdata, 0, NULL);
  return tdata;
}

VALUE 
rbc_task_to_json(VALUE self) {
  Task *task;
	Data_Get_Struct(self, Task, task);
  pstring *json = task_to_json(task);
  VALUE rstring = p2rstring(json);
  pstring_free(json);
  return rstring;
}
