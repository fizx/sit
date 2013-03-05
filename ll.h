#ifndef LL_H_INCLUDED
#define LL_H_INCLUDED

typedef struct LList {
  const void   *data;
  struct LList *next;
} LList;

void
ll_add(LList **ll, const void *data);

void
ll_free(LList *ll);

#endif