#include "sit.h"

vstring *
vstring_new() {
  return calloc(1, sizeof(vstring));
}

void
vstring_append(vstring *vstr, pstring *pstr) {
  VStringNode *node = malloc(sizeof(*node));
  node->strings = NULL;
  node->off = 0;
  if(vstr->node) {
    node->off = vstr->node->off + vstr->node->pstr.len;
  }
  node->prev = vstr->node;
  vstr->node = node;
  pstring_copulate(&node->pstr, pstr);
}

void
_v(pstring *out, VStringNode *node, long off) {
  if(!node) return;
  
  int len = out->len;
  long reloff = off - node->off;
  long relend = reloff + len;
  
  assert(reloff <= off + node->pstr.len);
  
  if (reloff >= 0 && relend <= node->pstr.len) {
    // the requested string exists entirely within one buffer, therefore
    // we do a pointer-only allocation and GTFO.
    out->val = node->pstr.val + reloff;
    return;
  } else if (relend < 0) {
    // the entire string has been filled already
    return;
  }
   
  if(reloff < 0) {
    // grab previous chunk
    _v(out, node->prev, off);
  } else {
    out->val = malloc(len);
  }
  
  // just need to fill in part of the string
  if (relend <= node->pstr.len) {
    
    // we own the end of this string, so register it
    ll_add(&node->strings, node->pstr.val);
  }

  long start = reloff > 0 ? reloff : 0; 
  long node_remaining = node->pstr.len - start;
  long string_remaining = len + (reloff < 0 ? reloff : 0); 
  long cplen = node_remaining > string_remaining ? string_remaining : node_remaining;
  memcpy((void *)(out->val - reloff), node->pstr.val + start, cplen);
}

void
vstring_get(pstring *target, vstring *vstr, long off) {
  if (off + target->len > vstring_size(vstr)) {
    target->len = -1;
  } else {
    _v(target, vstr->node, off + vstr->off);
  }
}

void 
vstring_node_free(VStringNode *node) {
  if(node->prev) {
    vstring_node_free(node->prev);
  }
  LList *ll = node->strings;
  LList *tmp;
  while(ll) {
    tmp = ll;
    ll = ll->next;
    ll_free(tmp);
  }
  free(node->pstr.val);
  free(node);
}

long
vstring_size(vstring *vstr) {
  if(vstr->node) {
    return vstr->node->off + vstr->node->pstr.len - vstr->off;
  } else {
    return 0;
  }
}

void
vstring_shift(vstring *vstr, long off) {
  vstr->off += off;
  VStringNode *node = vstr->node;
  VStringNode *next = NULL;
  while(node) {
    if(node->off + node->pstr.len < vstr->off) {
      if(next) next->prev = NULL;
      vstring_node_free(node);
      return;
    }
    next = node;
    node = node->prev;
  }
}

void
vstring_free(vstring *vstr) {
  if(vstr->node) vstring_node_free(vstr->node);
  free(vstr);
}