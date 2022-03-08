/*
** Interpreter state.
*/
#ifndef _SY_STATE_H
#define _SY_STATE_H

#include "object.h"

#define GLOBAL_ENV_SIZE 128
#define HEAP_SIZE 1024  // in cells

enum {
  E_OK,
  E_EOF,
  E_TOKEN,
  E_DOTSEP,
  E_RANGE,
};

struct SchemeY {
  /* global variables */
  // vector_t *globals;
  cell_t *globals;  // top-level environment
  cell_t *inport;  // current input port
  cell_t *outport;  // current output port
  
  /* registers */
  // cell_t *env;  // current environment
  // cell_t *eval;  // evaluation stack
  // cell_t *args;  // argument stack
  // cell_t *acc;  // accumulator

  /* managed heap */
  heap_t *heap;  // managed heap
  // cell_t *pin;  // temporary objects (for gc)

  /* reader state */
  char *tbase, *tptr, *tend;  // token buffer
  int lookahead;  // lookahead character
  size_t depth, lineno;  // reader state
  int prompt;  // interactive prompt mode

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