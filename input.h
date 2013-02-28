#ifndef INPUT_H_INCLUDED
#define INPUT_H_INCLUDED

#include "engine.h"
#include "query_parser.h"

typedef struct Output {
	void *data;
  void (*write)(struct Output *output, pstring *message);
  void (*close)(struct Output *output);
} Output;

typedef enum { REGISTERING, QUERYING } QueryParserMode;

typedef struct QueryIdNode {
  long query_id;
  struct QueryIdNode *next;
} QueryIdNode;

typedef struct Input {
  struct Engine      *engine;
	Parser      *parser;
  QueryParser *qparser;
  
  QueryParserMode qparser_mode;
  QueryIdNode    *query_ids;
  
  Output         *output;

	void *data;

} Input;

Input *
input_new(struct Engine *engine, long buffer_size);

void
input_consume(struct Input *input, pstring *pstr);

void 
input_end_stream(struct Input *input);

#endif