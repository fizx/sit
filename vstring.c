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
ll_add(LList **ll, const void *data) {
  LList *node = malloc(sizeof(LList));
  node->data = data;
  node->next = *ll;
  *ll = node;
}

void
ll_free(LList *ll) {
  if(ll->next) {
    ll_free(ll->next);
  }
  free(ll->data);
  free(ll);
}

pstring *
_v(VStringNode *node, long off, int len) {
  if(!node) {
    return NULL;
  }
  
  long reloff = off - node->off;
  long relend = reloff + len;
  
  assert(reloff <= off + node->pstr.len);
  
  if (reloff >= 0 && relend <= node->pstr.len) {
    // the requested string exists entirely within one buffer, therefore
    // we do a pointer-only allocation and GTFO.
    pstring *out = malloc(sizeof(pstring));
    out->val = node->pstr.val + reloff;
    out->len = len;
    return out;
  } else if (relend < 0) {
    // the entire string has been filled already
    return _v(node->prev, off, len);
  } else {
    pstring *out;
    if(reloff < 0) {
      // grab previous chunk
      out = _v(node->prev, off, len);
    } else {
      out = pstring_new(len);
    }
    
    if(out) {
      
      // just need to fill in part of the string
      if (relend <= node->pstr.len) {
        
        // we own the end of this string, so register it
        ll_add(&node->strings, node->pstr.val);
      }
    
      long start = reloff > 0 ? reloff : 0; 
      long node_remaining = node->pstr.len - start;
      long string_remaining = len + (reloff < 0 ? reloff : 0); 
      long cplen = node_remaining > string_remaining ? string_remaining : node_remaining;
      memcpy(out->val - reloff, node->pstr.val + start, cplen);
    }
    return out;
  } 
}

pstring *
vstring_get(vstring *vstr, long off, int len) {
  if(off + len > vstring_size(vstr)) return NULL;
  return _v(vstr->node, off + vstr->off, len);
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