#ifndef SIT_CALLBACK_H_INCLUDED
#define SIT_CALLBACK_H_INCLUDED

// A generic callback suitable for attaching to the perc query tree.
// This has a next field, so it's also a linked list node.
typedef struct _sit_callback {
  void                 (*handler)(void* sit_data, void *user_data);
  void                  *user_data;
  struct _sit_callback *next;
  int                    id;
  void                 (*free)(struct _sit_callback*);
} sit_callback;

sit_callback *
sit_callback_new();

void
sit_callback_free(sit_callback *);


#endif
