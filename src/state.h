/*
** Interpreter state.
**
**
*/
#ifndef _STATE_H
#define _STATE_H

#include "object.h"

#include <stdlib.h>

#define GLOBAL_ENV_SIZE 128

#define DEFAULT_INPUT_PORT stdin
#define DEFAULT_OUTPUT_PORT stdout

typedef struct State {

  Object *stack;
  Object *stack_top;
  size_t stack_size;

  Object *env_head;  /* Current environment */
  Object *env_tail;  /* Top-level environment */

  int err;  /* Error status */
} State;

State *state_new();
void state_free(State *s);

// Object *sy_lookup(State *s, Object *var);
// Object *sy_bind(State *s, Object *var, Object *val);

// Object *sy_intern(State *s, char *sym);
// Object *sy_intern_bind(State *s, char *sym, Object *val);

// Object *sy_eval(State *s, Object *expr);

#endif