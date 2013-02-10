#ifndef SERVER_RUBY_H_INCLUDED
#define SERVER_RUBY_H_INCLUDED

#include "sit_server.h"
#include "ruby.h"

VALUE
rbc_server_new(VALUE class, VALUE rengine);

VALUE 
rbc_server_start(VALUE self, VALUE rport);

#endif
