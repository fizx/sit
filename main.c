#include "sit.h"
#include "logger.h"
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <getopt.h>

#define BUF_SIZE 65536

void
usage() {
  puts("Usage: sit [--mem-size=<bytes>] [--log-file=<path>]");
  puts("           [--data-dir=<path>]  [--port=<num>]");
  puts("           [--help] [--test-mode]");
  exit(1);
}

void
stdout_write(sit_output *output, pstring *data) {
  (void) output;
  printf("%.*s\n", data->len, data->val);
}

void 
stdout_close(sit_output *output) {
  (void) output;
}

int 
main(int argc, char **argv) {
  int c;	
  
  int port = -1;
  const char *logfile = NULL;
  // const char *data    = NULL;
  long ram            = 10000000; // 10mb
  
  set_logger(stderr);
  
  while (1) {
    static struct option long_options[] = {
      {"help",      no_argument,       0, 'h'},
      {"port",      required_argument, 0, 'p'},
      {"log-file",  required_argument, 0, 'l'},
      {"data-dir",  required_argument, 0, 'd'},
      {"mem-size",  required_argument, 0, 'm'},
      {"test-mode", no_argument,       0, 't'},
      {0, 0, 0, 0}
    };
    
    int option_index = 0;
    c = getopt_long(argc, argv, "hp:l:d:r:t", long_options, &option_index);
    if (c == -1) break;
    
    switch (c) {
    case 'h':
    case '?':
      usage();
      break;
    case 'p':
      port = strtol(optarg, NULL, 10);
      break;
    case 'l':
      logfile = optarg;
      break;
    case 't':
      setTestMode(true);
      DEBUG("test-mode is engaged");
      break;
    case 'd':
      // TODO: care about persistance.
      break;
    case 'm':
      ram = strtol(optarg, NULL, 10);  
    }
  }
  set_logger(logfile ? fopen(logfile, "a") : stderr);
  
  sit_parser *parser = json_parser_new(white_parser_new());
  sit_engine *engine = sit_engine_new(parser, ram);

#ifdef HAVE_EV_H
  
	if(port > 0) {
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
  
  	sit_input *input = sit_input_new(engine, engine->term_capacity, BUF_SIZE);
    input->output = malloc(sizeof(sit_output));
    input->output->write = stdout_write;
    input->output->close = stdout_close;
    sit_protocol_parser *pparser = sit_line_input_protocol_new(input);
  
    char buffer[BUF_SIZE];
    pstring pstr;
    pstr.val = buffer;

    while(fgets(buffer, BUF_SIZE, stdin)) {
  		pstr.len = strlen(buffer);
      pparser->consume(pparser, &pstr);
  	}
  }
}
