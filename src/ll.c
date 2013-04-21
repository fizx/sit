#include "sit.h"

void
ll_add(LList **ll, const void *data) {
  LList *node = malloc(sizeof(LList));
  node->data = data;
  node->next = *ll;
  *ll = node;
}

void
ll_free(LList *ll) {
  if(!ll) return;
  ll_free(ll->next);
  if (ll->data) free((void *)ll->data);
  free(ll);
}