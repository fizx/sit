#include "sit.h"

#define BUF_SIZE 65536

void
usage() {
  puts("Usage: sit [--mem-size=<bytes>] [--log-file=<path>]");
  puts("           [--data-dir=<path>]  [--port=<num>]");
  puts("           [--help] [--test-mode]");
  exit(1);
}

void
stdout_write(Output *output, pstring *data) {
  (void) output;
  printf("%.*s\n", data->len, data->val);
}

void 
stdout_close(Output *output) {
  (void) output;
  exit(0);
}

int 
main(int argc, char **argv) {
  int c;	
  
  int port = -1;
  const char *logfile = NULL;
  // const char *data    = NULL;
  long ram            = 10000000; // 10mb
  bool solr = false;
  bool dedupe = false;
  pstring *data_dir = NULL;
  
  set_logger(stderr);
  
  while (1) {
    static struct option long_options[] = {
      {"help",      no_argument,       0, 'h'},
      {"port",      required_argument, 0, 'p'},
      {"log-file",  required_argument, 0, 'l'},
      {"data-dir",  required_argument, 0, 'd'},
      {"mem-size",  required_argument, 0, 'm'},
      {"test-mode", no_argument,       0, 't'},
      {"solr",      no_argument,       0, 's'},
      {"dedupe",    no_argument,       0, 'z'},
      {0, 0, 0, 0}
    };
    
    int option_index = 0;
    c = getopt_long(argc, argv, "hp:l:d:r:tsz", long_options, &option_index);
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
    case 's':
      solr = true;
      break;
    case 't':
      setTestMode(true);
      DEBUG("test-mode is engaged");
      break;
    case 'd':
      data_dir = c2pstring(optarg);
      break;
    case 'z':
      dedupe = true;
      break;
    case 'm':
      ram = strtol(optarg, NULL, 10);  
    }
  }
  if(logfile) {
    FILE *handle = fopen(logfile, "a");
    if(handle) {
      set_logger(handle);
    } else {
      PERROR("can't log to %s, using", logfile);
    }
  }
  
  Parser *parser = json_white_parser_new();
  Engine *engine = engine_new(parser, data_dir, ram, dedupe);

	if(port > 0) {
		struct sockaddr_in addr;
		bzero(&addr, sizeof(addr));
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = INADDR_ANY;
		
		Server *server = server_new(engine);
		server_start(server, &addr);
	} else {

    pstring nl = {"\n", 1};
  
  	Input *input = input_new(engine, BUF_SIZE);
    input->output = malloc(sizeof(Output));
    input->output->delimiter = &nl;
    input->output->write = stdout_write;
    input->output->close = stdout_close;
    ProtocolParser *pparser = line_input_protocol_new(input);
  
    char buffer[BUF_SIZE];
    pstring pstr;
    pstr.val = buffer;

    while(fgets(buffer, BUF_SIZE, stdin)) {
  		pstr.len = strlen(buffer);
      pparser->consume(pparser, &pstr);
  	}
    line_input_protocol_free(pparser);
    free(input->output);
  	input_free(input);
    engine_free(engine);
  }
  return 0;
}
