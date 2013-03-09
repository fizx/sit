#ifndef SIT_SERVER_H_INCLUDED
#define SIT_SERVER_H_INCLUDED
#ifdef HAVE_EV_H

#include <ev.h>
#include "engine.h"
#include "input.h"
struct Engine;
struct Input;

typedef struct {
	struct ev_io        as_io;
	struct ev_loop     *loop;
	struct Engine  *engine;
	struct sockaddr_in *addr;
	int                 total_clients;
} Server;

typedef struct {
	struct ev_io             as_io;
	Server                  *server;
	struct sockaddr_in      *addr;
	struct ProtocolParser   *parser;
  bool                     live;
} conn_t;

Server *
server_new(struct Engine *engine);

int
server_start(Server *server, struct sockaddr_in *addr);

conn_t *
conn_new(Server *server);

void 
conn_close(conn_t *conn);

void
conn_start(conn_t * conn, int revents);

void
conn_free(conn_t * conn);

#endif
#endif

