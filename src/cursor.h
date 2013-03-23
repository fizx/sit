#ifndef CURSOR_H_INCLUDED
#define CURSOR_H_INCLUDED

typedef struct Cursor {
  bool  (*prev)(struct Cursor *cursor);
  bool  (*next)(struct Cursor *cursor);
  long  (*id)(struct Cursor *cursor);
  long  (*seek_lte)(struct Cursor *cursor, long id);
  void   *data;
} Cursor;
  
#endif