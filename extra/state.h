/*
** Interpreter state.
*/
#ifndef _STATE_H
#define _STATE_H

#include "object.h"

#define GLOBAL_ENV_SIZE 128

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

struct State {
  /* global variables */
  Vector *globals;  // top-level environment
  Cell *inport;  // default input port
  Cell *outport;  // default output port

  /* registers */
  Cell *env;  // environment stack
  Cell *eval;  // current code
  Cell *args;  // closure arguments
  Cell *acc;  // accumulator

  Cell *stack_head;
  Cell *stack_tail;

  /* managed heap */
  Cell *heap, *heap2;  // semi-spaces
  Cell *next;  // next free space
  size_t semi;  // semi-space size

  /* reader state */
  char *token, *tp, *tend;  // token buffer
  int lookahead;  // lookahead character
  size_t depth, lineno;  // reader state
  int prompt;  // interactive prompt mode?
  
  /* error handling */
  int err;  // error status
};

void sy_init(State *s);
void sy_shutdown(State *s);

// Cell *syS_addfn(SchemeY *s, Cell *var, ffun *fn);

Cell *sy_lookup(State *s, Cell *var);
Cell *sy_bind(State *s, Cell *var, Cell *val);

Cell *sy_intern(State *s, char *sym);
Cell *sy_intern_bind(State *s, char *sym, Cell *val);

Cell *sy_eval(State *s, Cell *expr);

#endif