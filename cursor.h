#ifndef SIT_CURSOR_H_INCLUDED
#define SIT_CURSOR_H_INCLUDED

#include <stdbool.h>

typedef struct sit_cursor {
  bool  (*prev)(struct sit_cursor *cursor);
  bool  (*next)(struct sit_cursor *cursor);
  long  (*id)(struct sit_cursor *cursor);
  long  (*seek_lte)(struct sit_cursor *cursor, long id);
  void   *data;
} sit_cursor;
  
#endif