#include "sit.h"
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>


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
		
		sit_server *server = sit_server_new(engine);
		sit_server_start(server, &addr);
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
