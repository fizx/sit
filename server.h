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
} sit_server;

typedef struct {
	struct ev_io        as_io;
	sit_server         *server;
	struct sockaddr_in *addr;
	struct sit_protocol_parser   *parser;
} conn_t;

sit_server *
sit_server_new(struct Engine *engine);

int
sit_server_start(sit_server *server, struct sockaddr_in *addr);

conn_t *
conn_new(sit_server *server);

void 
conn_close(conn_t *conn);

void
conn_start(conn_t * conn, int revents);

void
conn_free(conn_t * conn);

#endif
#endif

