#include "sit.h"

static int task_id = 0;

Task *
task_new() {
  Task *task = malloc(sizeof(Task));
  task->id = task_id++;
  return task;
}

void
task_free(Task *task) {
  
}

#define TBUF_SIZE 16384

typedef struct TailState {
  FILE *fd;
  long pointer;
  long size;
  ev_stat *stat;
  char buffer[TBUF_SIZE];
  struct aiocb aiocbp;
  ev_timer *timer;
  long docs;
  long errors;
} TailState;

static void
_try_read(Task *task){
  TailState *state = task->state;
  struct aiocb *aiocbp = &state->aiocbp;
  if (state->size > state->pointer) {
    int status = aio_error(aiocbp);
    switch(status) {
      case EINVAL: { // ready to read more
        int left = state->size - state->pointer;
        int nbytes = TBUF_SIZE > left ? left : TBUF_SIZE;
      
        aiocbp->aio_offset = state->pointer;
        aiocbp->aio_nbytes = nbytes;
        aio_read(aiocbp);
        break;
      }
      case 0: { // success
        assert(state->pointer == aiocbp->aio_offset);
        ssize_t size = aio_return(aiocbp);
        pstring pstr = { state->buffer, size };
        task->parser->consume(task->parser, &pstr);
        state->pointer += size;
        //fall
      }
      case EINPROGRESS:{
    	  struct ev_loop *loop = ev_default_loop(0);
        ev_timer_again(loop, state->timer);
        break;
      }
      default:
        PERROR("unknown tail error");
        abort();
    }
  }
}

static void
_task_stat(EV_P_ ev_stat *w, int revents) {  
  Task *task = w->data;
  TailState *state = task->state;
  if(w->attr.st_size > state->size) {
    state->size = w->attr.st_size;
    _try_read(task);
  }
}

static void
_try_again(EV_P_ ev_timer *w, int revents) {  
  Task *task = w->data;
  _try_read(task);
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


Task *
tail_task_new(Engine *engine, pstring *path) {
  Task *task = task_new();
  task->engine = engine;
  task->parser = engine->parser->fresh_copy(engine->parser);
	task->parser->on_document = callback_new(_tail_document_found, task);
	task->parser->on_error = callback_new(_tail_error_found, task);
	task->data = NULL;
	
  TailState *state = calloc(1, sizeof(TailState));
  task->state = state;
	struct ev_loop *loop = ev_default_loop(0);
  char *name = p2cstring(path);
	state->fd = fopen(name, "r");
  if(!state->fd) {
    free(name);
    return NULL;
  }
  struct stat buf;
  int out = fstat(fileno(state->fd), &buf);
  if(out < 0) return NULL;
  
  state->timer = malloc(sizeof(ev_timer));
  ev_timer_init(state->timer, _try_again, 0., 0.);
  ev_timer_start(loop, state->timer);

  state->pointer = 0;
  state->size = buf.st_size;
  state->aiocbp.aio_buf = state->buffer;
  state->aiocbp.aio_fildes = fileno(state->fd);
  state->aiocbp.aio_sigevent.sigev_notify = SIGEV_NONE;
  ev_stat *task_stat = malloc(sizeof(ev_stat));
  state->stat = task_stat;
  task_stat->data = task;
  ev_stat_init(task_stat, _task_stat, name, 0);
  ev_stat_start(loop, task_stat);
  free(name);
  return task;
}