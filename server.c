#ifdef HAVE_EV_H
#include "sit.h"

#define BUFFER_SIZE 8096
#define STREAM_BUFFER_SIZE 1048576

void 
read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
  conn_t *conn = (conn_t *)watcher;
	char buffer[BUFFER_SIZE];
	ssize_t read;

  if(!conn->live) return;

	if(EV_ERROR & revents) {
  	PERROR("got invalid event");
  	return;
	}

	// Receive message from client socket
	read = recv(watcher->fd, buffer, BUFFER_SIZE, 0);

	if(read < 0) {
  	PERROR("read error");
  	return;
	} else {
    DEBUG("Read %ld bytes", read);
	}

	if(read == 0) {
    conn_close(conn);
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
	Server *server = (Server *) watcher;
	conn_t *conn = conn_new(server);
	conn_start(conn, revents);
}

void
conn_free(conn_t * conn) {
  Input *input = conn->parser->handler->data;
  free(input->output);
  input_free(input);
  line_input_protocol_free(conn->parser);
  vstring_free(conn->buffer);
	free(conn);
}

void
_enqueue_flush(conn_t *conn);
  
void
_conn_flush_cb(struct ev_loop *loop, struct ev_timer *timer, int revents) {
  conn_t *conn = timer->data;
  vstring *buffer = conn->buffer;
  
  long size = vstring_size(buffer);
  while(size) {
    if(size > BUFFER_SIZE) size = BUFFER_SIZE;
  
    pstring data = { NULL, size};
    vstring_get(&data, buffer, 0);
  
    int sent = send(conn->as_io.fd, data.val, data.len, 0);
    if(sent < 0) {
      switch(errno) {
        case EPIPE: // sigpipe
          conn_close(conn);
          break;
        case EAGAIN: // eagain
          conn->flusher = NULL;
          _enqueue_flush(conn);
          free(timer);
          return;
        default:
          PERROR("unknown error writing");
      }
      errno = 0;
      break;
    } else {
      vstring_shift(buffer, sent);
      size = vstring_size(buffer);
    }
  }
  conn->flusher = NULL;
  free(timer);
}


void
_enqueue_flush(conn_t *conn) {
  if(!conn->flusher) {
    struct ev_loop *loop = conn->server->loop;
    conn->flusher = malloc(sizeof(ev_timer));
    conn->flusher->data = conn;
    ev_timer_init(conn->flusher, _conn_flush_cb, 0., 0.);
    ev_timer_start(loop, conn->flusher);
  }
}

void
conn_write(Output *output, pstring *data) {  
  conn_t *conn = output->data;
  if(!conn->live) return;

  paddc(data, "\n");
  vstring_append(conn->buffer, data);
  
  _enqueue_flush(conn);
}

void
_conn_free_cb(struct ev_loop *loop, ev_timer *timer, int revents) {
  conn_free(timer->data);
  free(timer);
}

void 
conn_close(conn_t *conn) {
  // Idempotent close
  if(!conn->live) return;
  conn->live = false;
  
  struct ev_loop *loop = conn->server->loop;
  
  // It gets really ugly to free connection-specific resources
  // during a unit of work.  It's much better to set a timer for the
  // next tick, so that we don't interrupt work in progress.
  ev_timer *conn_free_timer = malloc(sizeof(ev_timer));
  conn_free_timer->data = conn;
  ev_timer_init(conn_free_timer, _conn_free_cb, 0., 0.);
  ev_timer_start(loop, conn_free_timer);
	
  DEBUG("closing: %d",conn->as_io.fd);
  ev_io_stop(ev_default_loop(0), &conn->as_io);
  close(conn->as_io.fd);
	conn->server->total_clients--;
	INFO("closing connection");
	INFO("%d client(s) connected.", conn->server->total_clients);
}


void 
out_conn_close(Output *output) {
  errno = 0;
  conn_t *conn = output->data;
  conn_close(conn);
}

conn_t *
conn_new(Server *server) {
  assert(server->engine);
	conn_t *conn = malloc(sizeof(*conn));
	conn->server = server;
	Input *input = input_new(server->engine, STREAM_BUFFER_SIZE);
  input->output = malloc(sizeof(Output));
  input->output->data = conn;
  input->output->write = conn_write;
  input->output->close = out_conn_close;
  conn->parser = line_input_protocol_new(input);
  conn->live = true;
  conn->buffer = vstring_new();
  conn->flusher = NULL;
	return conn;
}

void
conn_start(conn_t * conn, int revents) {
	struct ev_loop *loop = conn->server->loop;
  struct sockaddr_in client_addr;
  socklen_t client_len = sizeof(client_addr);
  int client_sd;
	
	if(EV_ERROR & revents) {
  	PERROR("got invalid event");
		conn_free(conn);
  	return;
	}
	
	client_sd = accept(conn->server->as_io.fd, (struct sockaddr *)&client_addr, &client_len);
  int flags = fcntl(client_sd, F_GETFL);
  flags |= O_NONBLOCK;
  fcntl(client_sd, F_SETFL, flags);
	
	if (client_sd < 0) {
	  PERROR("accept error");
		conn_free(conn);
	  return;
	}
 	conn->server->total_clients++;

  ev_io_init((struct ev_io *) conn, read_cb, client_sd, EV_READ);
  ev_io_start(loop, (struct ev_io *) conn);
	INFO("Successfully connected with client.");
	INFO("%d client(s) connected.", conn->server->total_clients);
}

Server *
server_new(Engine *engine) {
	Server *server = malloc(sizeof(*server));
	server->engine = engine;
	server->loop = ev_default_loop (0);
	server->addr = NULL;
	server->total_clients = 0;
  signal(SIGPIPE, SIG_IGN);
	return server;
}

int reuse = 1;

int
server_start(Server *server, struct sockaddr_in *addr) {
	struct ev_loop *loop = ev_default_loop(0);
	int sd;

	// Create server socket
	if( (sd = socket(PF_INET, SOCK_STREAM, 0)) < 0 ) {
	  PERROR("socket error");
	  return -1;
	}
		
	setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	// Bind socket to address
	if (bind(sd, (struct sockaddr*) addr, sizeof(*addr)) != 0) {
	  PERROR("bind error");
	  return -1;
	}
	
	// Start listing on the socket
	if (listen(sd, 2) < 0) {
	  PERROR("listen error");
	  return -1;
	}

	// Initialize and start a watcher to accepts client requests
	ev_io_init((struct ev_io *) server, accept_cb, sd, EV_READ);
	ev_io_start(loop, (struct ev_io *) server);

	// Start infinite loop
	INFO("Successfully started server.");	
  ev_loop(loop, 0);
  return 0;
}

#endif
