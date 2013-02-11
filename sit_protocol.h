#ifndef SIT_PROTOCOL_H_INCLUDED
#define SIT_PROTOCOL_H_INCLUDED

#include "pstring.h"
#include "sit.h"

typedef enum {
  COMPLETE, PARTIAL, FORCE_DATA
} sit_proto_state;

struct sit_input;
struct sit_protocol_handler;

typedef struct sit_protocol_parser {
  struct sit_protocol_handler *handler;
  void      (*consume)(struct sit_protocol_parser *protocol, pstring *pstr);
  void      (*end_stream)(struct sit_protocol_parser *protocol);
  sit_proto_state state;
  void       *data;
} sit_protocol_parser;

typedef struct sit_protocol_handler {
  sit_protocol_parser *parser;
  void (*command_found)(struct sit_protocol_handler *protocol, pstring *command, pstring* more);
  void (*data_found)(struct sit_protocol_handler *protocol, pstring *data);
  void (*data_complete)(struct sit_protocol_handler *protocol);
  void (*error_found)(struct sit_protocol_handler *protocol, pstring *data);
  void  *data;
} sit_protocol_handler;

sit_protocol_parser *
sit_line_protocol_new();

sit_protocol_parser *
sit_line_input_protocol_new(struct sit_input *input);

#endif
