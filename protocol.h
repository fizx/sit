#ifndef SIT_PROTOCOL_H_INCLUDED
#define SIT_PROTOCOL_H_INCLUDED

#include "pstring.h"
#include "sit.h"

typedef enum {
  COMPLETE, PARTIAL, FORCE_DATA
} ProtocolState;

struct Input;
struct ProtocolHandler;

typedef struct ProtocolParser {
  struct ProtocolHandler *handler;
  void      (*consume)(struct ProtocolParser *protocol, pstring *pstr);
  void      (*end_stream)(struct ProtocolParser *protocol);
  ProtocolState state;
  void       *data;
} ProtocolParser;

typedef struct ProtocolHandler {
  ProtocolParser *parser;
  void (*command_found)(struct ProtocolHandler *protocol, pstring *command, pstring* more);
  void (*data_found)(struct ProtocolHandler *protocol, pstring *data);
  void (*data_complete)(struct ProtocolHandler *protocol);
  void (*error_found)(struct ProtocolHandler *protocol, pstring *data);
  void  *data;
} ProtocolHandler;

ProtocolParser *
line_protocol_new();

ProtocolParser *
line_input_protocol_new(struct Input *input);

void
line_input_protocol_free(ProtocolParser *parser);

#endif
