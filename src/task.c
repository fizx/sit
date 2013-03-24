#include "sit.h"

static long task_id = 0;

Task *
task_new() {
  Task *task = calloc(1, sizeof(Task));
  task->id = task_id++;
  return task;
}

void
task_free(Task *task) {
  
}

long
task_last_id() {
  return task_id - 1;
}


#define TBUF_SIZE 16384

typedef struct TailState {
  FILE *ptr;
  pstring *path;
  long off;
  long size;
  ev_stat *stat;
  char buffer[TBUF_SIZE];
  struct aiocb aiocbp;
  ev_timer *timer;
  long docs;
  long errors;
} TailState;

typedef struct ClientState {
	struct ev_io as_io;
  vstring     *write_buffer;
  int          fd;
  pstring     *endpoint;
  long         docs;
  long         errors;
  Task        *task;
  bool         connected;
} ClientState;

pstring *
task_to_json(Task *task) {
  if(!task) return pstring_new2("null", 4);
  if(task->to_json) {
    return task->to_json(task);
  } else {
    char *buf;
    asprintf(&buf, "{\"id\": %d, \"type\": \"unknown\"}");
    pstring *str = malloc(sizeof(pstring));
    str->val = buf;
    str->len = strlen(buf);
    return str;
  }
}

static void
_retry_task(Task *task, bool now) {
  DEBUG("retrying tail");
  struct ev_loop *loop = ev_default_loop(0);
  TailState *state = task->state;
  
  struct stat buf;
  int out = fstat(fileno(state->ptr), &buf);
  if(out < 0) return;
  state->size = buf.st_size;
  DEBUG("believe size: %d, offset: %d", state->size, state->off);
  ev_timer_set(state->timer, now ? 1. : 0., 0.);
  ev_timer_start(loop, state->timer);
}

static void
_try_read(Task *task){
  TailState *state = task->state;
  struct aiocb *aiocbp = &state->aiocbp;
  if (state->size > state->off) {
    errno = 0;
    int status = aio_error(aiocbp);
    switch(errno) {
      case EINVAL: { // ready to read more
        int left = state->size - state->off;
        int nbytes = TBUF_SIZE > left ? left : TBUF_SIZE;
      
        aiocbp->aio_offset = state->off;
        aiocbp->aio_nbytes = nbytes;
        aio_read(aiocbp);
        _retry_task(task, true);
        break;
      }
      case 0: { // success
        if(status != EINPROGRESS) {
          assert(state->off == aiocbp->aio_offset);
          ssize_t size = aio_return(aiocbp);
          pstring pstr = { state->buffer, size };
          task->parser->consume(task->parser, &pstr);
          state->off += size;
      	}
      	_retry_task(task, true);
        break;
      }
      default:
        PERROR("unknown tail error");
        // abort();
    }
  }
}

static void
_task_stat(EV_P_ ev_stat *w, int revents) {  
  Task *task = w->data;
  TailState *state = task->state;
  DEBUG("task_stat");
  DEBUG("believe size: %d, offset: %d", state->size, state->off);
  if(w->attr.st_size > state->size) {
    state->size = w->attr.st_size;
    _try_read(task);
  } else {
    _retry_task(task, false);
  }
}

static void
_try_again(EV_P_ ev_timer *w, int revents) {  
  Task *task = w->data;
  _try_read(task);
}

void 
_client_document_found(Callback *cb, void *data) {
  Task *task = cb->user_data;
	Engine *engine = task->engine;
  engine->current_output = NULL;
  engine_document_found(engine, data);
  ClientState *state = task->state;
  state->docs++;
}

void 
_client_error_found(Callback *cb, void *data) {
  Task *task = cb->user_data;
  ClientState *state = task->state;
  state->errors++;
}

void 
_tail_document_found(Callback *cb, void *data) {
  Task *task = cb->user_data;
	Engine *engine = task->engine;
  engine->current_output = NULL;
  engine_document_found(engine, data);
  TailState *state = task->state;
  state->docs++;
}

void 
_tail_error_found(Callback *cb, void *data) {
  Task *task = cb->user_data;
  TailState *state = task->state;
  state->errors++;
}

static pstring *
_tail_task_to_json(Task *task) {
  TailState *state = task->state;
  char *buf;
  pstring escaped;
  json_escape(&escaped, state->path);
  asprintf(&buf, "{\"id\": %d, \"type\": \"tail\", \"path\": \"%.*s\", \"docs\": %d, \"errors\": %d, \"offset\": %d}", 
    task->id, escaped.len, escaped.val, state->docs, state->errors, state->off);
  pstring *str = malloc(sizeof(pstring));
  str->val = buf;
  str->len = strlen(buf);
  return str;
}

static pstring *
_client_task_to_json(Task *task) {
  ClientState *state = task->state;
  char *buf;
  pstring escaped;
  json_escape(&escaped, state->endpoint);
  asprintf(&buf, 
    "{\"id\": %d, \"type\": \"client\", \"endpoint\": \"%.*s\", \"connected\": %s}", 
    task->id, escaped.len, escaped.val, state->connected ? "true": "false");
  pstring *str = malloc(sizeof(pstring));
  str->val = buf;
  str->len = strlen(buf);
  return str;
}

Task *
tail_task_new(Engine *engine, pstring *more, double interval) {
  char *space = memchr(more->val, ' ', more->len);
  if(!space) return NULL;
  int pathlen = space - more->val;
  pstring path = { more->val, pathlen };
  pstring after = { more->val + pathlen + 1, more->len - (pathlen + 1) };
  if(after.len < 1) {
    puts("a");
    return NULL;
  }
  Task *task = task_new();
  task->engine = engine;
  task->parser = engine_new_stream_parser(engine, &after);
  if(!task->parser) {
    task_free(task);
    printf("%.*s\n", after.len, after.val);
    puts("b");
    return NULL;
  }
	task->parser->on_document = callback_new(_tail_document_found, task);
	task->parser->on_error = callback_new(_tail_error_found, task);
	task->data = NULL;
  task->to_json = _tail_task_to_json;

  TailState *state = calloc(1, sizeof(TailState));
  task->state = state;
	struct ev_loop *loop = ev_default_loop(0);
  char *name = p2cstring(&path);
	state->ptr = fopen(name, "r");
  if(!state->ptr) {
    task_free(task);
    free(name);
    return NULL;
  }
  struct stat buf;
  int out = fstat(fileno(state->ptr), &buf);
  if(out < 0) return NULL;
  
  state->timer = malloc(sizeof(ev_timer));
  state->timer->data = task;  
  ev_timer_init(state->timer, _try_again, 0., 0.);
  ev_timer_start(loop, state->timer);

  state->off = 0;
  state->size = buf.st_size;
  state->aiocbp.aio_buf = state->buffer;
  state->aiocbp.aio_fildes = fileno(state->ptr);
  state->aiocbp.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
  ev_stat *task_stat = malloc(sizeof(ev_stat));
  state->stat = task_stat;
  task_stat->data = task;
  DEBUG("tail interval: %f");
  ev_stat_init(task_stat, _task_stat, name, interval);
  ev_stat_start(loop, task_stat);
  state->path = pcpy(&path);
  dictAdd(engine->tasks, task, task);
  free(name);
  return task;
}

#define BUFFER_SIZE 16384
static char _buf[BUFFER_SIZE];

static void 
_client_cb(EV_P_ ev_io *w, int revents) {
  ClientState *state = (ClientState *)w;
  Task *task = state->task;
  
  if (revents & EV_WRITE) { // we're connected
    state->connected = true;
    
    // Quit sending still-connected notices
    vstring *buffer = state->write_buffer;
    long size = vstring_size(buffer);
    if(size > BUFFER_SIZE) size = BUFFER_SIZE;
    if (size > 0) {
      pstring data = { NULL, size};
      vstring_get(&data, buffer, 0);
      DEBUG("sending tell: %.*s", data.len, data.val);
      int sent = send(state->fd, data.val, data.len, 0);
      if(sent < 0) {
        switch(errno) {
        case EPIPE: // sigpipe
          task_free(task);
          return;
        case EAGAIN: // eagain
          DEBUG("EAGAIN");
          return;
        default:
          PERROR("unknown error writing");
          task_free(task);
          return;
        }
        errno = 0;
      } else {  
        DEBUG("sent %d bytes", sent);
        vstring_shift(buffer, sent);
      }
    } else { //nothing to do, quit trying
      ev_io_stop(EV_A_ w);
      ev_io_set(w, state->fd, EV_READ);
      ev_io_start(EV_A_ w);
    }
  } else if (revents & EV_READ) { // data available
    int read = recv(state->fd, _buf, BUFFER_SIZE, 0);
    if (read > 0) {
      pstring pstr = { _buf, read };
      task->parser->consume(task->parser, &pstr);
    } else if (read == 0) {
      engine_release_task(task->engine, task->id);
    } else if (EAGAIN == errno) {
      DEBUG("WTF");
    } else {
      PERROR("unknown client error");
      engine_release_task(task->engine, task->id);
    }
  } else {
    DEBUG("WTF");
  }
}

void
_client_tell(Task *task, pstring *message) {
  struct ev_loop *loop = EV_DEFAULT;
  ClientState *state = task->state;
  DEBUG("telling: %.*s", message->len, message->val);
  vstring_append(state->write_buffer, message);
  ev_io_stop(EV_A_ state);
  ev_io_set((struct ev_io *)state, state->fd, EV_READ | EV_WRITE);
  ev_io_start(EV_A_ state);
}

Task *
client_task_new(Engine *engine, pstring *hostport) {
  Task *task = task_new();
  task->engine = engine;
  task->parser = engine->parser->fresh_copy(engine->parser);
	task->parser->on_document = callback_new(_client_document_found, task);
	task->parser->on_error = callback_new(_client_error_found, task);
  task->to_json = _client_task_to_json;
  task->tell = _client_tell;
  ClientState *state = calloc(1, sizeof(ClientState));
  task->state = state;
  state->task = task;
  state->endpoint = pcpy(hostport);
  state->write_buffer = vstring_new();

  if (-1 == (state->fd = socket(AF_INET, SOCK_STREAM, 0))) {
    PERROR("socket");
    task_free(task);
    return NULL;
  }
  
  int flags;
  flags = fcntl(state->fd, F_GETFL);
  flags |= O_NONBLOCK;
  int status = fcntl(state->fd, F_SETFL, flags);
  if(status < 0) {
    PERROR("can't set client socket nonblock");
    task_free(task);
    return NULL;
  }

  // this should be initialized before the connect() so
  // that no packets are dropped when initially sent?
  // http://cr.yp.to/docs/connect.html

  struct ev_loop *loop = EV_DEFAULT;
  ev_io_init((ev_io *)state, _client_cb, state->fd, EV_READ | EV_WRITE);
  ev_io_start(EV_A_ (ev_io *)state);
  
  char *colon = memchr(hostport->val, ':', hostport->len);
  if(!colon) {
    PERROR("invalid host:port");
    task_free(task);
    return NULL;
  }
  int off = colon - hostport->val;
  char *host = pstring_cslice(hostport, 0, off);
  char *port = pstring_cslice(hostport, off + 1, hostport->len - (off + 1));
  struct addrinfo hint;
  memset((void *)&hint, 0, sizeof(hint));
  struct addrinfo *addr = NULL;
  if(-1 == getaddrinfo(host, port, NULL, &addr)) {
    PERROR("can't resolve");
    task_free(task);
    return NULL;    
  }
  
  addr->ai_addr->sa_family = AF_INET;

  if (-1 == connect(state->fd, addr->ai_addr, addr->ai_addrlen)) {
    if(errno == EINPROGRESS) {
      errno = 0;
    } else {
      PERROR("can't connect");
      task_free(task);
      return NULL;
    }
  }
  dictAdd(engine->tasks, task, task);
  return task;
}
