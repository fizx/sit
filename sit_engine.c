#include "sit_engine.h"
#include "ring_buffer.h"
#include "dict.h"
#include "dict_types.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>

typedef struct sit_query_node {
	dict                   *children;
	sit_term							 *term;
	struct sit_query_node  *parent;
	sit_callback 				   *callback;
} sit_query_node;

typedef struct unregister_data {
  long query_id;
  bool success;
} unregister_data;

sit_engine *
sit_engine_new(sit_parser *parser, long size) {
	int tc = size / 64;
 	sit_engine *engine = malloc(sizeof(sit_engine) + (tc - 1) * (sizeof(sit_term)));
	engine->queries = dictCreate(getTermDict(), 0);
	engine->parser = parser;
	engine->query_id = 0;
	engine->on_document_found = NULL;
	parser->receiver = &engine->as_receiver;
  parser->receiver->term_found = sit_engine_term_found;
  parser->receiver->document_found = sit_engine_document_found;
  parser->receiver->field_found = sit_engine_field_found;
  parser->receiver->int_found = sit_engine_int_found;
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

sit_term SENTINEL = {
  NULL,
  NULL,
  0,
  0,
  true, 
  false
};

int 
_recurse_add(sit_engine *engine, dict *hash, sit_query_node *parent, sit_term *term, int remaining, bool negated_yet, sit_callback *callback) {
  assert(term);
	pstring *tmp;
	if(!term->hash_code) {
		sit_term_update_hash(term);
	}

	if(term->negated && !negated_yet) {
	  sit_query_node *node = dictFetchValue(hash, &SENTINEL);
	  if (node == NULL) {
  	  node = calloc(sizeof(sit_query_node), 1);
  		node->parent = parent;
  		node->term = &SENTINEL;
  		node->children = dictCreate(getTermDict(), 0);
      dictAdd(hash, &SENTINEL, node);
		}
    parent = node;
    hash = node->children;
    negated_yet = true;
	}
	
	sit_query_node *node = dictFetchValue(hash, term);
	if (node == NULL) {
		node = calloc(sizeof(sit_query_node), 1);
		node->parent = parent;
		node->term = term;
		dictAdd(hash, term, node);
		tmp = sit_term_to_s(term);
	}
	if (remaining == 1) {
		sit_callback *next = node->callback;
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
sit_engine_register(sit_engine *engine, sit_query *query) {
  for(int i = 0; i< query->count; i++) {
	  _recurse_add(engine, engine->queries, NULL, query->conjunctions[i]->terms, query->conjunctions[i]->count, false, query->callback);
  }
  return query->callback->id;
}

sit_term **
_get_terms(sit_query_node *node, int *term_count) {	 

  if(node->term != &SENTINEL){
    (*term_count)++;
  }    
  
  int depth = *term_count;

  if(node->parent) {
    sit_term **terms = _get_terms(node->parent, term_count);
    terms[*term_count - depth] = node->term;
    return terms;
  } else {
    sit_term **terms = malloc(sizeof(sit_term*) * (*term_count));
    terms[0] = node->term;
    return terms;
  }
}

void
_recurse_each(sit_callback *cb, dict *hash) {
	dictIterator * iterator = dictGetIterator(hash);
	dictEntry *next;
	while((next = dictNext(iterator))) {
		sit_query_node *node = dictGetVal(next);
		cb->handler(cb, node);
		if(node->children) {
			_recurse_each(cb, node->children);
		}
	}
}

void
_each_query(sit_callback *cb, void *vnode) {
	sit_query_node *node = vnode;
  sit_callback *user_callback = cb->user_data;
	
	sit_callback *qc = node->callback;
  sit_term **terms = NULL;
	int term_count = 0;
	while(qc) {
		if (terms == NULL) {
			terms = _get_terms(node, &term_count);
		}
    conjunction_t *cjs[1];
    cjs[0] = conjunction_new(terms, term_count);
		sit_query *query = sit_query_new(cjs, 1, qc);
		user_callback->handler(user_callback, query);
		qc = qc->next;
	}
}

void 
sit_engine_each_query(sit_engine *engine, sit_callback *callback) {
	sit_callback wrapper;
	wrapper.user_data = callback;
	wrapper.handler = _each_query;
	sit_engine_each_node(engine, &wrapper);
}

void 
sit_engine_each_node(sit_engine *engine, sit_callback *callback) {
	_recurse_each(callback, engine->queries);
}

pstring *
sit_engine_last_document(sit_engine *engine) {
	long id = sit_engine_last_document_id(engine);
 	pstring *out = sit_engine_get_document(engine, id);
	return out;
}

pstring *
sit_engine_get_document(sit_engine *engine, long doc_id) {
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
sit_engine_last_document_id(sit_engine *engine) {
  return engine->docs->written / sizeof(doc_ref) - 1;
}

void 
callback_recurse(sit_engine *engine, dict *term_index, dict *query_nodes, void *doc, bool positive) {
	assert(doc);
  if(positive) {
    sit_query_node *n = dictFetchValue(query_nodes, &SENTINEL);
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
		  	sit_query_node *node = dictGetVal(next);
  			sit_callback *cb = node->callback;
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
		sit_query_node *node;

    while ((next = dictNext(iterator))) {
      if(positive == !!(node = dictFetchValue(query_nodes, dictGetKey(next)))) {
  			sit_callback *cb = node->callback;
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
sit_engine_percolate(sit_engine *engine) {
  long id = sit_engine_last_document_id(engine);
	callback_recurse(engine, INPUTISH(term_index), engine->queries, &id, true);
}

void
sit_engine_index(sit_engine *engine, long doc_id) {
	for (int i = 0; i < INPUTISH(term_count); i++) {
		sit_term *term = &INPUTISH(terms)[i];
		plist *value = lrw_dict_get(engine->term_dictionary, term);
		if(value == NULL) {
			value = plist_new(engine->postings);
			lrw_dict_put(engine->term_dictionary, sit_term_copy(term), value);
			assert(lrw_dict_get(engine->term_dictionary, term));
		} else {
      lrw_dict_tap(engine->term_dictionary, term);
			assert(lrw_dict_get(engine->term_dictionary, term));
		}
    plist_entry entry = { doc_id, term->offset };
		plist_append_entry(value, &entry);
	}
}

void
_unregister_handler(sit_callback *cb, void *vnode) {
	sit_query_node *node = vnode;
	unregister_data *data = cb->user_data;
	
	if(data->success) {
    return;
	}
	
	// handle the first case
	while (node->callback && node->callback->id == data->query_id) {
		sit_callback *old = node->callback;
		node->callback = old->next;
		if(old->free) {
			old->free(old);
		}
    data->success = true;
    return;
	}
	
	sit_callback *prev = node->callback;
	sit_callback *qc = node->callback;
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

sit_result_iterator *
sit_engine_search(sit_engine *engine, sit_query *query) {
  long max = sit_engine_last_document_id(engine)+1;
  sit_result_iterator *iter = malloc(sizeof(sit_result_iterator));
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
      sit_term *term = &cj->terms[j];
      sit_cursor *cursor = dictFetchValue(iter->cursors, term);
      if(cursor == NULL) {
        if(term->numeric) {
          ring_buffer *rb = dictFetchValue(engine->ints, term->field);
          cursor = rb == NULL ? NULL : &ring_buffer_predicate_int_cursor_new(rb, sizeof(int), term->text->val[0], term->offset)->as_cursor;
        } else {
          plist *pl = lrw_dict_get(engine->term_dictionary, term);
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
sit_result_sub_iterator_prev(sub_iterator *iter) {
  int size = iter->count;
  long min = iter->doc_id;
  min--;
  while (min >= 0) {
    long max = -1;
    for (int i = 0; i < size; i++) {
      sit_cursor *cursor = iter->cursors[i];
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
sit_result_iterator_prev(sit_result_iterator *iter) {
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
      sit_result_sub_iterator_prev(subiter);
    }
    
    while(subiter->doc_id > bound) {
      sit_result_sub_iterator_prev(subiter);
    }
    
    if(subiter->doc_id > max) max = subiter->doc_id;
  }

  iter->initialized = true;
  iter->doc_id = max;
  
  return max >= (long)0;
}

void
sit_result_iterator_do_callback(sit_result_iterator *iter) {
  long id = sit_result_iterator_document_id(iter);
  iter->query->callback->handler(iter->query->callback, &id);
}

pstring *
sit_result_iterator_document(sit_result_iterator *iter) {
  return sit_engine_get_document(iter->engine, sit_result_iterator_document_id(iter));
}

long
sit_result_iterator_document_id(sit_result_iterator *iter) {
  return iter->doc_id;
}

bool
sit_engine_unregister(sit_engine *engine, long query_id) {
  unregister_data data = {
    query_id,
    false
  };
	sit_callback unregister;
	unregister.user_data = &data;
	unregister.handler = _unregister_handler;
	sit_engine_each_node(engine, &unregister);
  return data.success;
}

void
sit_engine_consume(sit_engine *engine, pstring *pstr) {
  ring_buffer_append_pstring(engine->stream, pstr);
	engine->parser->consume(engine->parser, pstr);
}

void 
sit_engine_term_found(sit_receiver *receiver, pstring *pstr, int field_offset) {
  sit_engine *engine = (sit_engine *)receiver;
	sit_term *term = &engine->terms[engine->term_count++];
	term->field = engine->field;
	term->text = pcpy(pstr);
	term->offset = field_offset;
	sit_term_update_hash(term);
	dictAdd(engine->term_index, term, term);
  DEBUG("sit_engine_term_found %.*s->%.*s", term->field->len, term->field->val, term->text->len, term->text->val);
}

void
sit_engine_zero_ints(sit_engine *engine) {  
  pstring *orig_field = engine->field;
  dictIterator *iter = dictGetIterator(engine->ints);
  dictEntry *entry;
  while ((entry = dictNext(iter))) {
    engine->field = dictGetKey(entry);
    sit_engine_int_found(&engine->as_receiver, 0);
  }
  engine->field = orig_field;
  dictReleaseIterator(iter);
}

void 
sit_engine_document_found(sit_receiver *receiver, long off, int len) {
  sit_engine *engine = (sit_engine *)receiver;
	assert(off >= 0);
	assert(len > 0);
  doc_ref dr = { off, len };
	ring_buffer_append(engine->docs, &dr, sizeof(dr));
  long doc_id = sit_engine_last_document_id(engine);
  if(engine->on_document_found) {
    engine->on_document_found->handler(engine->on_document_found, engine);
  }
	sit_engine_percolate(engine);
	sit_engine_index(engine, doc_id);
  sit_engine_zero_ints(engine);
	engine->term_count = 0;
	dictEmpty(engine->term_index);
}

int *
sit_engine_get_int(sit_engine *engine, long doc_id, pstring *field) {
  ring_buffer *rb = dictFetchValue(engine->ints, field);
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
sit_engine_set_int(sit_engine *engine, long doc_id, pstring *field, int value) {
  //FIXME?!
  ring_buffer *rb = dictFetchValue(engine->ints, field);
  assert(field);
  if(rb == NULL) {
    rb = ring_buffer_new(engine->ints_capacity);
    dictAdd(engine->ints, field, rb);
  }
  ring_buffer_put(rb, doc_id * sizeof(int), &value, sizeof(int));
}

void
sit_engine_incr(sit_engine *engine, long doc_id, pstring *field, int value) {
  int *ptr = sit_engine_get_int(engine, doc_id, field);
  if(ptr != NULL) {
    *ptr += value;
  }
}

void 
sit_engine_int_found(sit_receiver *receiver, int value) {
  sit_engine *engine = (sit_engine *)receiver;
  long doc_id = sit_engine_last_document_id(engine) + 1;
  sit_engine_set_int(engine, doc_id, engine->field, value);
}

void 
sit_engine_field_found(sit_receiver *receiver, pstring *name) {
  sit_engine *engine = (sit_engine *)receiver;
	engine->field = name;
}
