#include "sit.h"
#ifdef HAVE_EV_H
# include <ev.h>
# include <sys/socket.h>
# include <stdio.h>
# include <netinet/in.h>

#define BUFFER_SIZE 1024

int total_clients = 0;  // Total number of connected clients


/* Read client message */
void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents){
char buffer[BUFFER_SIZE];
ssize_t read;

if(EV_ERROR & revents)
{
  perror("got invalid event");
  return;
}

// Receive message from client socket
read = recv(watcher->fd, buffer, BUFFER_SIZE, 0);

if(read < 0)
{
  perror("read error");
  return;
}

if(read == 0)
{
  // Stop and free watchet if client socket is closing
  ev_io_stop(loop,watcher);
  free(watcher);
  perror("peer might closing");
  total_clients --; // Decrement total_clients count
  printf("%d client(s) connected.\n", total_clients);
  return;
}
else
{
  printf("message:%s\n",buffer);
}

// Send message bach to the client
send(watcher->fd, buffer, read, 0);
bzero(buffer, read);
}




/* Accept client requests */
void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents)
{
struct sockaddr_in client_addr;
socklen_t client_len = sizeof(client_addr);
int client_sd;
struct ev_io *w_client = (struct ev_io*) malloc (sizeof(struct ev_io));

if(EV_ERROR & revents)
{
  perror("got invalid event");
  return;
}

// Accept client request
client_sd = accept(watcher->fd, (struct sockaddr *)&client_addr, &client_len);

if (client_sd < 0)
{
  perror("accept error");
  return;
}

total_clients ++; // Increment total_clients count
printf("Successfully connected with client.\n");
printf("%d client(s) connected.\n", total_clients);

// Initialize and start watcher to read client requests
ev_io_init(w_client, read_cb, client_sd, EV_READ);
ev_io_start(loop, w_client);
}

#endif

#define BUF_SIZE 65536

sit_query *query = NULL;
sit_engine *engine = NULL;

void 
_print_handler(void *sit_data, void *user_data) {
	(void) user_data;
	pstring *pstr = sit_data;
	printf("%ld\t%.*s\n", sit_engine_last_document_id(engine), pstr->len, pstr->val);
}


void
_main_query_handler(void *sit_data, void *user_data) {
	(void) user_data;
	query = sit_data;
	query->callback = sit_callback_new();
	query->callback->handler = _print_handler;
}

int 
main(int argc, char **argv) {
	if(argc != 3) {
		printf("Usage: sit FILE QUERY \n");
		printf("Usage: sit --server PORT \n");
		return 1;
	}
	char *filename = argv[1];
	char *str = argv[2];
#ifdef HAVE_EV_H
	if(!strcmp(filename, "--server")) {
		int port = atoi(str);
		if(!port) {
			perror("invalid port");
		  return -1;
		}

		struct sockaddr_in addr;
		bzero(&addr, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = INADDR_ANY;

		struct ev_loop *loop = ev_default_loop(0);
		int sd;
		struct ev_io w_accept;

		// Create server socket
		if( (sd = socket(PF_INET, SOCK_STREAM, 0)) < 0 )
		{
		  perror("socket error");
		  return -1;
		}

		// Bind socket to address
		if (bind(sd, (struct sockaddr*) &addr, sizeof(addr)) != 0)
		{
		  perror("bind error");
		}

		// Start listing on the socket
		if (listen(sd, 2) < 0)
		{
		  perror("listen error");
		  return -1;
		}

		// Initialize and start a watcher to accepts client requests
		ev_io_init(&w_accept, accept_cb, sd, EV_READ);
		ev_io_start(loop, &w_accept);

		// Start infinite loop
		while (1)
		{
		  ev_loop(loop, 0);
		}		
	} else {
#else
  {
#endif
		sit_callback *cb = sit_callback_new();
		cb->handler = _print_handler;
	
		sit_parser *parser = json_parser_new(white_parser_new());
		engine = sit_engine_new(parser, 100000);
		query_parser *qparser = query_parser_new();
		qparser->cb = sit_callback_new();
		qparser->cb->handler = _main_query_handler;
		query_parser_consume(qparser, c2pstring(str));
		if(!query) {
			if(qparser->error) {
				printf("Could not recognize your query: %.*s\n", qparser->error->len, qparser->error->val);
			} else {
				printf("Could not recognize your query: unknown error\n");
			}
			return 2;
		}
	
		pstring *ps = sit_query_to_s(query);
		fprintf(stderr, "query: %.*s\n", ps->len, ps->val);
	
		sit_engine_register(engine, query);

		pstring pstr;
		char buffer[BUF_SIZE];	
		pstr.val = buffer;
	
		FILE *file;
		if(!strcmp(filename, "-")) {
			file = stdin;
		} else {
			file = fopen(filename, "r");
		}
	
		while(fgets(buffer, BUF_SIZE, file)) {
			pstr.len = strlen(buffer);
			sit_engine_consume(engine, &pstr);
		}
	}
}
