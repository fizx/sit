#ifndef SIT_PROTOCOL_H_INCLUDED
#define SIT_PROTOCOL_H_INCLUDED

#include "pstring.h"

typedef struct sit_protocol {
  void      (*consume)(struct sit_protocol *protocol, pstring *pstr);
  void      (*command_found)(struct sit_protocol *protocol, pstring *command, int argc, pstring** argv);
  void      (*data_found)(struct sit_protocol *protocol, pstring *data);
  sit_input  *input;
  void       *state;
  void       *data;
} sit_protocol;

sit_protocol *
sit_line_protocol_new(sit_input *input);

#endif
