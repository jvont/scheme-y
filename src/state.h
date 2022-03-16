/*
** Interpreter state.
*/
#ifndef _STATE_H
#define _STATE_H

#include "object.h"

#define GLOBAL_ENV_SIZE 8
#define HEAP_SIZE 32

enum {
  E_OK,
  E_EOF,
  E_TOKEN,
  E_DOTSEP,
  E_RANGE,
};

struct SchemeY {
  /* global variables */
  vector_t *globals;  // top-level environment
  cell_t *inport;  // default input port
  cell_t *outport;  // default output port

  /* registers */
  cell_t *env;  // environment stack
  cell_t *eval;  // current code
  cell_t *args;  // closure arguments
  cell_t *acc;  // accumulator

  /* managed heap */
  // heap_t *g0, *g1;
  cell_t *heap, *heap2;  // semi-spaces
  cell_t *next;  // next free space
  size_t semi;  // semi-space size

  /* reader state */
  char *token, *tp, *tend;  // token buffer
  int lookahead;  // lookahead character
  size_t depth, lineno;  // reader state
  int prompt;  // interactive prompt mode?
  
  /* error handling */
  int err;  // error status
};

void sy_init(SchemeY *s);
void sy_shutdown(SchemeY *s);

// cell_t *syS_addfn(SchemeY *s, cell_t *var, ffun *fn);

cell_t *sy_lookup(SchemeY *s, cell_t *var);
cell_t *sy_bind(SchemeY *s, cell_t *var, cell_t *val);

cell_t *sy_intern(SchemeY *s, char *sym);
cell_t *sy_intern_bind(SchemeY *s, char *sym, cell_t *val);

cell_t *sy_eval(SchemeY *s, cell_t *expr);

#endif