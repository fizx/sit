#include "sit.h"

static unsigned int 
_term_hash(const void *key) {
  return ((Term *) key)->hash_code;
}

static int 
_term_compare(void *privdata, const void *key1, const void *key2) {
  (void) privdata;
  return _term_hash(key1) == _term_hash(key2);
}

static void
_term_bump(dictEntry *entry, long value) {
	Term *term = dictGetKey(entry);
	term->offset = value;
}

static long
_term_version(dictEntry *entry) {
	Term *term = dictGetKey(entry);
	return term->offset;
}

static unsigned int 
_pstr_hash(const void *key) {  
  pstring *pstr = (pstring *) key;
  return pstrhash(pstr);
}

static int 
_pstr_compare(void *privdata, const void *key1,
        const void *key2) {
  (void) privdata;
  pstring *a = (pstring *) key1;
  pstring *b = (pstring *) key2;
  return pstrcmp(a, b) == 0;
}

void *
_term_copy(void *privdata, const void *term) {
  (void) privdata;
  return term_copy(term);
}

void
_term_free(void *privdata, void *term) {
  (void) privdata;
  return term_free(term);
}

void 
_pstr_free(void *privdata, void *pstr) {
  (void) privdata;
  pstring_free(pstr);
}

void 
_plist_free(void *privdata, void *pl) {
  (void) privdata;
  plist_free(pl);
}

void *
_pstr_dup(void *privdata, const void *pstr) {
  (void) privdata;
  return pcpy(pstr);
}

void
_rb_free(void *privdata, const void *rb) {
  (void) privdata;
  ring_buffer_free(rb);
}

dictType pstrDict = {
    _pstr_hash,    /* hash function */
    _pstr_dup,     /* key dup */
    NULL,          /* val dup */
    _pstr_compare, /* key compare */
    _pstr_free,    /* key destructor */
    NULL           /* val destructor */
};

dictType pstrRingBufferDict = {
    _pstr_hash,    /* hash function */
    _pstr_dup,     /* key dup */
    NULL,          /* val dup */
    _pstr_compare, /* key compare */
    _pstr_free,    /* key destructor */
    _rb_free       /* val destructor */
};

dictType termDict = {
    _term_hash,    /* hash function */
    NULL,          /* key dup */
    NULL,          /* val dup */
    _term_compare, /* key compare */
    NULL,    			 /* key destructor */
    NULL           /* val destructor */
};

dictType termTermDict = {
    _term_hash,    /* hash function */
    NULL,          /* key dup */
    NULL,          /* val dup */
    _term_compare, /* key compare */
    NULL,    			 /* key destructor */
    NULL           /* val destructor */
};

dictType termPlistDict = {
    _term_hash,    /* hash function */
    _term_copy,    /* key dup */
    NULL,          /* val dup */
    _term_compare, /* key compare */
    _term_free,    /* key destructor */
    _plist_free    /* val destructor */
};

lrw_type termLrw = {
	_term_bump,
	_term_version
};


dictType *getPstrDict() { return &pstrDict; }
lrw_type *getTermLrw() { return &termLrw; }
dictType *getTermDict() { return &termDict; }
dictType *getTermTermDict() { return &termTermDict; }
dictType *getPstrRingBufferDict() { return &pstrRingBufferDict; }
dictType *getTermPlistDict() { return &termPlistDict; }

