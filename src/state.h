/*
** Interpreter state.
*/
#ifndef _STATE_H
#define _STATE_H

#include "object.h"

#define GLOBAL_ENV_SIZE 128
#define HEAP_SIZE 1024

enum {
  E_OK,
  E_EOF,
  E_TOKEN,
  E_DOTSEP,
  E_RANGE,
};

// enum {
//   CONST_T,
//   CONST_F,
//   CONST_EOF,
// };

struct SchemeY_ {
  /* global variables */
  vector *globals;  // top-level environment
  cell *inport;  // default input port
  cell *outport;  // default output port

  /* registers */
  cell *env;  // environment stack
  cell *eval;  // current code
  cell *args;  // closure arguments
  cell *acc;  // accumulator

  /* managed heap */
  // heap_t *g0, *g1;
  cell *heap, *heap2;  // semi-spaces
  cell *next;  // next free space
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

// cell *syS_addfn(SchemeY *s, cell *var, ffun *fn);

cell *sy_lookup(SchemeY *s, cell *var);
cell *sy_bind(SchemeY *s, cell *var, cell *val);

cell *sy_intern(SchemeY *s, char *sym);
cell *sy_intern_bind(SchemeY *s, char *sym, cell *val);

cell *sy_eval(SchemeY *s, cell *expr);

#endif