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

void
_parser_free(void *privdata, void *parser) {
  (void) privdata;
  parser_free(parser);
}

static void *
_term_next(void *data) {
	Term *term = term;
  return term->next;
}

static void
_term_set_next(void *data, void *next) {
	Term *term = data;
  term->next = next;
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

static unsigned int 
_docref_hash(const void *key) {  
  DocRef *dr = (DocRef *) key;
  return dr->hash_code;
}

static int 
_pstr_compare(void *privdata, const void *key1,
        const void *key2) {
  (void) privdata;
  pstring *a = (pstring *) key1;
  pstring *b = (pstring *) key2;
  return pstrcmp(a, b) == 0;
}

static int 
_docref_cmp(void *privdata, const void *key1, const void *key2) {
  Engine *engine = privdata;
  
  DocRef *a = key1;
  DocRef *b = key2;
  
  pstring *pa = a->tmp ? a->tmp : ring_buffer_get_pstring(engine->stream, a->off, a->len);
  pstring *pb = b->tmp ? b->tmp : ring_buffer_get_pstring(engine->stream, b->off, b->len);
  
  return pstrcmp(pa, pb) == 0;
}

void *
_term_copy(void *privdata, const void *term) {
  (void) privdata;
  return term_copy(term);
}

void
_term_free(void *privdata, void *term) {
  (void) privdata;
  term_free(term);
}

void
_void_free(void *privdata, void *data) {
  (void) privdata;
  free(data);
}

void 
_plist_cursor_free(void *privdata, void *cursor) {
  (void) privdata;
  plist_cursor_free(cursor);
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

void 
_query_node_free(void *privdata, const void *data) {
  (void) privdata;
  QueryNode *node = data;
  query_node_free(node);
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

dictType termQueryNodeDict = {
    _term_hash,    /* hash function */
    NULL,    /* key dup */
    NULL,          /* val dup */
    _term_compare, /* key compare */
    _term_free,    /* key destructor */
    _query_node_free     /* val destructor */
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

dictType termPlistCursorDict = {
    _term_hash,    /* hash function */
    _term_copy,    /* key dup */
    NULL,          /* val dup */
    _term_compare, /* key compare */
    _term_free,    /* key destructor */
    _plist_cursor_free    /* val destructor */
};

dictType pstrParserDict = {
    _pstr_hash,    /* hash function */
    _pstr_dup,     /* key dup */
    NULL,          /* val dup */
    _pstr_compare, /* key compare */
    _pstr_free,    /* key destructor */
    _parser_free   /* val destructor */
};

dictType docrefDict = {
    _docref_hash,    /* hash function */
    NULL,     /* key dup */
    NULL,          /* val dup */
    _docref_cmp, /* key compare */
    NULL,    /* key destructor */
    NULL   /* val destructor */
};

lrw_type termLrw = {
	_term_next,
	_term_set_next
};

dictType *getPstrDict() { return &pstrDict; }
lrw_type *getTermLrw() { return &termLrw; }
dictType *getTermDict() { return &termDict; }
dictType *getTermTermDict() { return &termTermDict; }
dictType *getPstrRingBufferDict() { return &pstrRingBufferDict; }
dictType *getTermPlistDict() { return &termPlistDict; }
dictType *getTermQueryNodeDict() { return &termQueryNodeDict; }
dictType *getTermPlistCursorDict() { return &termPlistCursorDict; }
dictType *getPstrParserDict() { return &pstrParserDict; }
dictType *getDocRefDict() { return &docrefDict; }
