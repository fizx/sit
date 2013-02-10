#ifdef HAVE_EV_H

#include "sit_server.h"
#include <ev.h>
#include <assert.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>

#define BUFFER_SIZE 8096
#define STREAM_BUFFER_SIZE 1048576

void 
read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
	conn_t *conn = (conn_t *)watcher;
	char buffer[BUFFER_SIZE];
	ssize_t read;

	if(EV_ERROR & revents) {
  	perror("got invalid event");
  	return;
	}

	// Receive message from client socket
	read = recv(watcher->fd, buffer, BUFFER_SIZE, 0);

	if(read < 0) {
  	perror("read error");
  	return;
	}

	if(read == 0) {
	  // Stop and free watchet if client socket is closing
  	ev_io_stop(loop, watcher);
  	conn->server->total_clients --; // Decrement total_clients count
		conn_free(conn);
  	perror("closing connection");
  	printf("%d client(s) connected.\n", conn->server->total_clients);
  	return;
	} else {
		pstring pstr;
		pstr.val = buffer;
		pstr.len = read;
    conn->parser->consume(conn->parser, &pstr);
		bzero(buffer, read);
	}
}

void 
accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
	(void) loop;
	sit_server *server = (sit_server *) watcher;
	conn_t *conn = conn_new(server);
	conn_start(conn, revents);
}

void
conn_free(conn_t * conn) {
	free(conn);
}

conn_t *
conn_new(sit_server *server) {
  assert(server->engine);
	conn_t *conn = malloc(sizeof(*conn));
	conn->server = server;
	sit_input *input = sit_input_new(server->engine, server->engine->term_capacity, STREAM_BUFFER_SIZE);
  conn->parser = sit_line_input_protocol_new(input);
	return conn;
}

void
conn_start(conn_t * conn, int revents) {
	struct ev_io *watcher = (struct ev_io *)conn;
	struct ev_loop *loop = conn->server->loop;
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_sd;
	
	if(EV_ERROR & revents) {
  	perror("got invalid event");
		conn_free(conn);
  	return;
	}
	
	client_sd = accept(conn->server->as_io.fd, (struct sockaddr *)&client_addr, &client_len);
	
	if (client_sd < 0) {
	  perror("accept error");
		conn_free(conn);
	  return;
	}
 	conn->server->total_clients++;

  ev_io_init((struct ev_io *) conn, read_cb, client_sd, EV_READ);
  ev_io_start(loop, (struct ev_io *) conn);
	printf("Successfully connected with client.\n");
	printf("%d client(s) connected.\n", conn->server->total_clients);
}

sit_server *
sit_server_new(sit_engine *engine) {
	sit_server *server = malloc(sizeof(*server));
	server->engine = engine;
	server->loop = ev_default_loop (0);
	server->addr = NULL;
	server->total_clients = 0;
	return server;
}

int
sit_server_start(sit_server *server, struct sockaddr_in *addr) {
	struct ev_loop *loop = ev_default_loop(0);
	int sd;

	// Create server socket
	if( (sd = socket(PF_INET, SOCK_STREAM, 0)) < 0 ) {
	  perror("socket error");
	  return -1;
	}

	// Bind socket to address
	if (bind(sd, (struct sockaddr*) addr, sizeof(*addr)) != 0) {
	  perror("bind error");
	}

	// Start listing on the socket
	if (listen(sd, 2) < 0) {
	  perror("listen error");
	  return -1;
	}

	// Initialize and start a watcher to accepts client requests
	ev_io_init((struct ev_io *) server, accept_cb, sd, EV_READ);
	ev_io_start(loop, (struct ev_io *) server);

	// Start infinite loop
	printf("Successfully started server.\n");	
	while (1)
	{
	  ev_loop(loop, 0);
	}	
}



#endif
