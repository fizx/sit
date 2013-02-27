#include "sit.h"

typedef struct QueryNode {
	dict                   *children;
	Term							 *term;
	struct QueryNode  *parent;
	Callback 				   *callback;
} QueryNode;

typedef struct unregister_data {
  long query_id;
  bool success;
} unregister_data;

Engine *
engine_new(Parser *parser, long size) {
	int tc = size / 64;
 	Engine *engine = malloc(sizeof(Engine) + (tc - 1) * (sizeof(Term)));
	engine->queries = dictCreate(getTermDict(), 0);
	engine->parser = parser;
	engine->query_id = 0;
	engine->on_document_found = NULL;
	parser->receiver = &engine->as_receiver;
  parser->receiver->term_found = engine_term_found;
  parser->receiver->document_found = engine_document_found;
  parser->receiver->field_found = engine_field_found;
  parser->receiver->int_found = engine_int_found;
	engine->stream = ring_buffer_new(size / 4);
	engine->term_dictionary = lrw_dict_new(getTermDict(), getTermLrw(), size / 32);
	engine->postings = plist_pool_new(size / 4);
	engine->term_index = dictCreate(getTermDict(), 0);
	engine->term_count = 0;
	engine->field = c2pstring("default");
	engine->docs = ring_buffer_new((size / 4 / sizeof(doc_ref)) * sizeof(doc_ref));
	engine->term_capacity = tc;
	engine->data = NULL;
	engine->ints_capacity = size / 4;
	engine->ints = dictCreate(getPstrDict(), 0);
	return engine;
}

Term SENTINEL = {
  NULL,
  NULL,
  0,
  0,
  true, 
  false
};

int 
_recurse_add(Engine *engine, dict *hash, QueryNode *parent, Term *term, int remaining, bool negated_yet, Callback *callback) {
  assert(term);
	pstring *tmp;
	if(!term->hash_code) {
		term_update_hash(term);
	}

	if(term->negated && !negated_yet) {
	  QueryNode *node = dictFetchValue(hash, &SENTINEL);
	  if (node == NULL) {
  	  node = calloc(sizeof(QueryNode), 1);
  		node->parent = parent;
  		node->term = &SENTINEL;
  		node->children = dictCreate(getTermDict(), 0);
      dictAdd(hash, &SENTINEL, node);
		}
    parent = node;
    hash = node->children;
    negated_yet = true;
	}
	
	QueryNode *node = dictFetchValue(hash, term);
	if (node == NULL) {
		node = calloc(sizeof(QueryNode), 1);
		node->parent = parent;
		node->term = term;
		dictAdd(hash, term, node);
		tmp = term_to_s(term);
	}
	if (remaining == 1) {
		Callback *next = node->callback;
		node->callback = callback;
		callback->next = next;
    if(callback->id < 0) callback->id = engine->query_id++;
		return callback->id;
	} else {
		if(node->children == NULL) {
			node->children = dictCreate(getTermDict(), 0);
		}
		return _recurse_add(engine, node->children, node, term + 1, remaining - 1, negated_yet, callback);
	}
}

long
engine_register(Engine *engine, Query *query) {
  for(int i = 0; i< query->count; i++) {
	  _recurse_add(engine, engine->queries, NULL, query->conjunctions[i]->terms, query->conjunctions[i]->count, false, query->callback);
  }
  return query->callback->id;
}

Term **
_get_terms(QueryNode *node, int *term_count) {	 

  if(node->term != &SENTINEL){
    (*term_count)++;
  }    
  
  int depth = *term_count;

  if(node->parent) {
    Term **terms = _get_terms(node->parent, term_count);
    terms[*term_count - depth] = node->term;
    return terms;
  } else {
    Term **terms = malloc(sizeof(Term*) * (*term_count));
    terms[0] = node->term;
    return terms;
  }
}

void
_recurse_each(Callback *cb, dict *hash) {
	dictIterator * iterator = dictGetIterator(hash);
	dictEntry *next;
	while((next = dictNext(iterator))) {
		QueryNode *node = dictGetVal(next);
		cb->handler(cb, node);
		if(node->children) {
			_recurse_each(cb, node->children);
		}
	}
}

void
_each_query(Callback *cb, void *vnode) {
	QueryNode *node = vnode;
  Callback *user_callback = cb->user_data;
	
	Callback *qc = node->callback;
  Term **terms = NULL;
	int term_count = 0;
	while(qc) {
		if (terms == NULL) {
			terms = _get_terms(node, &term_count);
		}
    conjunction_t *cjs[1];
    cjs[0] = conjunction_new(terms, term_count);
		Query *query = query_new(cjs, 1, qc);
		user_callback->handler(user_callback, query);
		qc = qc->next;
	}
}

void 
engine_each_query(Engine *engine, Callback *callback) {
	Callback wrapper;
	wrapper.user_data = callback;
	wrapper.handler = _each_query;
	engine_each_node(engine, &wrapper);
}

void 
engine_each_node(Engine *engine, Callback *callback) {
	_recurse_each(callback, engine->queries);
}

pstring *
engine_last_document(Engine *engine) {
	long id = engine_last_document_id(engine);
 	pstring *out = engine_get_document(engine, id);
	return out;
}

pstring *
engine_get_document(Engine *engine, long doc_id) {
  if (doc_id < 0) {
    return NULL;
  }
  long off = doc_id * sizeof(doc_ref);
  doc_ref *dr = ring_buffer_get(engine->docs, off, sizeof(doc_ref));
  if(dr == NULL) {
    return NULL;
  } else {
		pstring *doc = ring_buffer_get_pstring(engine->stream, dr->off, dr->len);
		return doc;
  }
}

long
engine_last_document_id(Engine *engine) {
  return engine->docs->written / sizeof(doc_ref) - 1;
}

void 
callback_recurse(Engine *engine, dict *term_index, dict *query_nodes, void *doc, bool positive) {
	assert(doc);
  if(positive) {
    QueryNode *n = dictFetchValue(query_nodes, &SENTINEL);
    if (n) {
      DEBUG("recursing SENTINEL");
      callback_recurse(engine, term_index, n->children, doc, false);
    }
  }
	if(dictSize(term_index) > dictSize(query_nodes) || !positive){
		dictIterator *iterator = dictGetIterator(query_nodes);
		dictEntry *next;
	
    while ((next = dictNext(iterator))) {
      if (positive == !!dictFetchValue(term_index, dictGetKey(next))) {
		  	QueryNode *node = dictGetVal(next);
  			Callback *cb = node->callback;
        DEBUG("perc'd %.*s:%.*s", node->term->field->len, node->term->field->val, node->term->text->len, node->term->text->val);
  			while(cb) {
  			  DEBUG("running callback %d", cb->id);
  				cb->handler(cb, doc);
  				cb = cb->next;
  			}
  			if(node->children) {
  				callback_recurse(engine, term_index, node->children, doc, positive);
  			}
  		}
		}
	} else {
		dictIterator *iterator = dictGetIterator(term_index);
		dictEntry *next;
		QueryNode *node;

    while ((next = dictNext(iterator))) {
      if(positive == !!(node = dictFetchValue(query_nodes, dictGetKey(next)))) {
  			Callback *cb = node->callback;
  			DEBUG("perc'd %.*s:%.*s", node->term->field->len, node->term->field->val, node->term->text->len, node->term->text->val);
  			while(cb) {
  			  DEBUG("running callback %d", cb->id);
  				cb->handler(cb, doc);
  				cb = cb->next;
  			}
  			if(node->children){
  				callback_recurse(engine, term_index, node->children, doc,positive);
  			}
  		}
		}
	}
}

#define INPUTISH(m) (engine->current_input ? engine->current_input->m : engine->m)

void
engine_percolate(Engine *engine) {
  long id = engine_last_document_id(engine);
	callback_recurse(engine, INPUTISH(term_index), engine->queries, &id, true);
}

void
engine_index(Engine *engine, long doc_id) {
	for (int i = 0; i < INPUTISH(term_count); i++) {
		Term *term = &INPUTISH(terms)[i];
		Plist *value = lrw_dict_get(engine->term_dictionary, term);
		if(value == NULL) {
			value = plist_new(engine->postings);
			lrw_dict_put(engine->term_dictionary, term_copy(term), value);
			assert(lrw_dict_get(engine->term_dictionary, term));
		} else {
      lrw_dict_tap(engine->term_dictionary, term);
			assert(lrw_dict_get(engine->term_dictionary, term));
		}
    PlistEntry entry = { doc_id, term->offset };
		plist_append_entry(value, &entry);
	}
}

void
_unregister_handler(Callback *cb, void *vnode) {
	QueryNode *node = vnode;
	unregister_data *data = cb->user_data;
	
	if(data->success) {
    return;
	}
	
	// handle the first case
	while (node->callback && node->callback->id == data->query_id) {
		Callback *old = node->callback;
		node->callback = old->next;
		if(old->free) {
			old->free(old);
		}
    data->success = true;
    return;
	}
	
	Callback *prev = node->callback;
	Callback *qc = node->callback;
	while(qc) {
		if(qc->id == data->query_id) {
			prev->next = qc->next;
			if(qc->free) {
				qc->free(qc);
			}
			data->success = true;
      return;
		}
		prev = qc;
		qc = qc->next;
	}
}

ResultIterator *
engine_search(Engine *engine, Query *query) {
  long max = engine_last_document_id(engine)+1;
  ResultIterator *iter = malloc(sizeof(ResultIterator));
  iter->query = query;
  if(query->callback->id < 0) query->callback->id = engine->query_id++;
  iter->engine = engine;
  iter->doc_id = max;
  iter->initialized = false;
  iter->cursors = dictCreate(getTermDict(), 0);
  iter->subs = malloc(sizeof(sub_iterator*) * query->count);
  iter->count = query->count;
  for(int i = 0; i < query->count; i++) {
    conjunction_t *cj = query->conjunctions[i];
    iter->subs[i] = malloc(sizeof(sub_iterator));
    iter->subs[i]->doc_id = max;
    iter->subs[i]->cursors = malloc(sizeof(plist_cursor*) * cj->count);
    iter->subs[i]->negateds = malloc(cj->count * sizeof(int));
    iter->subs[i]->state = malloc(sizeof(long) * cj->count);
    iter->subs[i]->initialized = false;
    iter->subs[i]->count = cj->count;
    for(int j = 0; j < cj->count; j++) {
      Term *term = &cj->terms[j];
      Cursor *cursor = dictFetchValue(iter->cursors, term);
      if(cursor == NULL) {
        if(term->numeric) {
          RingBuffer *rb = dictFetchValue(engine->ints, term->field);
          cursor = rb == NULL ? NULL : &ring_buffer_predicate_int_cursor_new(rb, sizeof(int), term->text->val[0], term->offset)->as_cursor;
        } else {
          Plist *pl = lrw_dict_get(engine->term_dictionary, term);
          cursor = pl == NULL ? NULL : &plist_cursor_new(pl)->as_cursor;
          dictAdd(iter->cursors, term, cursor);
        }
      }
      iter->subs[i]->negateds[j] = (int) term->negated;
      iter->subs[i]->cursors[j] = cursor;
      iter->subs[i]->state[j] = max;
    }
  }
  return iter; 
}

// TODO: not that efficient
bool
result_sub_iterator_prev(sub_iterator *iter) {
  int size = iter->count;
  long min = iter->doc_id;
  min--;
  while (min >= 0) {
    long max = -1;
    for (int i = 0; i < size; i++) {
      Cursor *cursor = iter->cursors[i];
      int negated = iter->negateds[i];

      if (cursor == NULL) {
        if(negated) {
          continue;
        } else {
          iter->doc_id = -1;
          return false;
        }
      }

      if(!iter->initialized) {
        iter->state[i] = LONG_MAX;
        cursor->prev(cursor);
      }
      
      if (negated) {
        long lower;
        while((lower = cursor->id(cursor)) >= min || iter->state[i] >= min) {
          iter->state[i] = lower;
          if(!cursor->prev(cursor)) {
            lower = -1;
            break;
          }
        }  

        min = lower - 1;
        if(min > max) max = min;
      } else {
        
        long doc = cursor->seek_lte(cursor, min);
        if(doc < min) min = doc;
        if(doc > max) max = doc;
      }
    }
    if(max == -1) {
      iter->doc_id = -1;
      return false;
    }
    iter->initialized = true;
    if(min == max) break;
  }
  
  iter->doc_id = min;
  return min >= 0;
}

// TODO: not that efficient
bool
result_iterator_prev(ResultIterator *iter) {
  int size = iter->count;

  long bound = iter->doc_id;
  bound--;
  long max = -1;
  for (int i = 0; i < size; i++) {
    sub_iterator *subiter = iter->subs[i];
    if (subiter == NULL) {
      iter->doc_id = -1;
      return false;
    }

    if(!iter->initialized) {
      result_sub_iterator_prev(subiter);
    }
    
    while(subiter->doc_id > bound) {
      result_sub_iterator_prev(subiter);
    }
    
    if(subiter->doc_id > max) max = subiter->doc_id;
  }

  iter->initialized = true;
  iter->doc_id = max;
  
  return max >= (long)0;
}

void
result_iterator_do_callback(ResultIterator *iter) {
  long id = result_iterator_document_id(iter);
  iter->query->callback->handler(iter->query->callback, &id);
}

pstring *
result_iterator_document(ResultIterator *iter) {
  return engine_get_document(iter->engine, result_iterator_document_id(iter));
}

long
result_iterator_document_id(ResultIterator *iter) {
  return iter->doc_id;
}

bool
engine_unregister(Engine *engine, long query_id) {
  unregister_data data = {
    query_id,
    false
  };
	Callback unregister;
	unregister.user_data = &data;
	unregister.handler = _unregister_handler;
	engine_each_node(engine, &unregister);
  return data.success;
}

void
engine_consume(Engine *engine, pstring *pstr) {
  ring_buffer_append_pstring(engine->stream, pstr);
	engine->parser->consume(engine->parser, pstr);
}

void 
engine_term_found(Receiver *receiver, pstring *pstr, int field_offset) {
  Engine *engine = (Engine *)receiver;
	Term *term = &engine->terms[engine->term_count++];
	term->field = engine->field;
	term->text = pcpy(pstr);
	term->offset = field_offset;
	term_update_hash(term);
	dictAdd(engine->term_index, term, term);
  DEBUG("engine_term_found %.*s->%.*s", term->field->len, term->field->val, term->text->len, term->text->val);
}

void
engine_zero_ints(Engine *engine) {  
  pstring *orig_field = engine->field;
  dictIterator *iter = dictGetIterator(engine->ints);
  dictEntry *entry;
  while ((entry = dictNext(iter))) {
    engine->field = dictGetKey(entry);
    engine_int_found(&engine->as_receiver, 0);
  }
  engine->field = orig_field;
  dictReleaseIterator(iter);
}

void 
engine_document_found(Receiver *receiver, long off, int len) {
  Engine *engine = (Engine *)receiver;
	assert(off >= 0);
	assert(len > 0);
  doc_ref dr = { off, len };
	ring_buffer_append(engine->docs, &dr, sizeof(dr));
  long doc_id = engine_last_document_id(engine);
  if(engine->on_document_found) {
    engine->on_document_found->handler(engine->on_document_found, engine);
  }
	engine_percolate(engine);
	engine_index(engine, doc_id);
  engine_zero_ints(engine);
	engine->term_count = 0;
	dictEmpty(engine->term_index);
}

int *
engine_get_int(Engine *engine, long doc_id, pstring *field) {
  RingBuffer *rb = dictFetchValue(engine->ints, field);
  long off = doc_id * sizeof(int);
  if(rb == NULL ||
    (off + sizeof(int) > rb->written) ||
    (off < rb->written - rb->capacity)
    ) {
    return NULL;
  } else {
    return (int *) &rb->buffer[off % rb->capacity];
  }
}

void
engine_set_int(Engine *engine, long doc_id, pstring *field, int value) {
  //FIXME?!
  RingBuffer *rb = dictFetchValue(engine->ints, field);
  assert(field);
  if(rb == NULL) {
    rb = ring_buffer_new(engine->ints_capacity);
    dictAdd(engine->ints, field, rb);
  }
  ring_buffer_put(rb, doc_id * sizeof(int), &value, sizeof(int));
}

void
engine_incr(Engine *engine, long doc_id, pstring *field, int value) {
  int *ptr = engine_get_int(engine, doc_id, field);
  if(ptr != NULL) {
    *ptr += value;
  }
}

void 
engine_int_found(Receiver *receiver, int value) {
  Engine *engine = (Engine *)receiver;
  long doc_id = engine_last_document_id(engine) + 1;
  engine_set_int(engine, doc_id, engine->field, value);
}

void 
engine_field_found(Receiver *receiver, pstring *name) {
  Engine *engine = (Engine *)receiver;
	engine->field = name;
}
