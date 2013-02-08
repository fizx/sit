#ifndef SIT_SERVER_H_INCLUDED
#define SIT_SERVER_H_INCLUDED
#ifdef HAVE_EV_H

#include <ev.h>
#include "sit_engine.h"
#include "sit_input.h"
struct sit_engine;
struct sit_input;

typedef struct {
	struct ev_io        io;
	struct ev_loop     *loop;
	struct sit_engine  *engine;
	struct sockaddr_in *addr;
	int                 total_clients;
} server_t;

typedef struct {
	struct ev_io        io;
	server_t           *server;
	struct sockaddr_in *addr;
	struct sit_input   *input;
} conn_t;

server_t *
server_new(struct sit_engine *engine);

int
server_start(server_t *server, struct sockaddr_in *addr);

conn_t *
conn_new(server_t *server);

void
conn_start(conn_t * conn, int revents);

void
conn_free(conn_t * conn);


#endif
#endif

