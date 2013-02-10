#include "server_ruby.h"
#include "util_ruby.h"
#include "sit.h"
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>

VALUE
rbc_server_new(VALUE class, VALUE rengine) {  
  sit_engine *engine;
  Data_Get_Struct(rengine, sit_engine, engine);
  sit_server *server = sit_server_new(engine);
	VALUE tdata = Data_Wrap_Struct(class, markall, NULL, server);
	rb_obj_call_init(tdata, 0, NULL);
  return tdata;
}

VALUE 
rbc_server_start(VALUE self, VALUE rport) {
  sit_server *server;
  Data_Get_Struct(self, sit_server, server);
  int port = NUM2INT(rport);
  
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	
  sit_server_start(server, &addr);
  return Qnil;
}
