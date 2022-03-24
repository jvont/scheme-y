/*
** Interpreter state.
**
** Each thread state object stores environment frames, a run queue (code), and
** evaluation stack. Each of these structures are added as roots to the
** managed heap.
**
** The goal of this layout is to facilitate a model for concurrent execution.
**
** Environments are pairs, where car points to current bindings and cdr points
** to the next environment pair to search for a specific interned symbol. The
** current environment is added as a root for collection.
** The top-level environment (env_tail) is shared between threads.
*/
#ifndef _STATE_H
#define _STATE_H

#include "object.h"

#include <stdlib.h>

#define GLOBAL_ENV_SIZE 128

#define DEFAULT_INPUT_PORT stdin
#define DEFAULT_OUTPUT_PORT stdout

typedef struct Thread {

  Object *queue_head;
  Object *queue_tail;

  Object *stack;
  Object *stack_top;
  size_t stack_size;

  Object *env_head;  /* Current environment rib */
  Object *env_tail;  /* Top-level table */

  int err;  /* Error status */
} Thread;

typedef struct State {

  Thread *threads;
  size_t threads_len;
  size_t threads_size;

  Thread *current_thread;

  // TODO: add heap data structures

} State;

Thread *thread_new();
void thread_free(Thread *s);

Object *lookup(Object *env);
Object *bind(Object *env, Object *var, Object *val);

Object *intern(Object *env, const char *s);

#endif