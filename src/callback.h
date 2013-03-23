#ifndef CALLBACK_H_INCLUDED
#define CALLBACK_H_INCLUDED

// A generic callback suitable for attaching to the perc query tree.
// This has a next field, so it's also a linked list node.
typedef struct Callback {
  void                 (*handler)(struct Callback *self, void *data);
  void                  *user_data;
  struct Callback       *next;
  long                   id;
  void                 (*free)(struct Callback* cb);
} Callback;

Callback *
callback_new(void (*handler)(struct Callback *self, void *data), void *user_data);

void
callback_free(Callback *);

#endif
