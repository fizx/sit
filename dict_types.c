#include "dict_types.h"
#include "sit.h"

static unsigned int 
_term_hash(const void *key)
{
    return ((sit_term *) key)->hash_code;
}

static int 
_term_compare(void *privdata, const void *key1,
        const void *key2)
{
    DICT_NOTUSED(privdata);
    return _term_hash(key1) == _term_hash(key2);
}

static void
_term_bump(dictEntry *entry, long value) {
	sit_term *term = dictGetKey(entry);
	term->offset = value;
}

static long
_term_version(dictEntry *entry) {
	sit_term *term = dictGetKey(entry);
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

dictType pstrDict = {
    _pstr_hash,    /* hash function */
    NULL,          /* key dup */
    NULL,          /* val dup */
    _pstr_compare, /* key compare */
    NULL,    			 /* key destructor */
    NULL           /* val destructor */
};

dictType termDict = {
    _term_hash,    /* hash function */
    NULL,          /* key dup */
    NULL,          /* val dup */
    _term_compare, /* key compare */
    NULL,    			 /* key destructor */
    NULL           /* val destructor */
};

lrw_type termLrw = {
	_term_bump,
	_term_version
};


dictType *getPstrDict() { return &pstrDict; }
lrw_type *getTermLrw() { return &termLrw; }
dictType *getTermDict() { return &termDict; }
