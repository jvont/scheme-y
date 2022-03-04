/*
** Interpreter state.
*/
#ifndef _SY_STATE_H
#define _SY_STATE_H

#include "object.h"
#include "mem.h"

#define GLOBAL_ENV_SIZE 128
#define HEAP_SIZE 1024

struct SchemeY {
  cell_t *global_env;  // top-level environment
  cell_t *env;  // environment stack
  cell_t *input_port;  // current input port
  cell_t *output_port;  // current output port

  Heap *heap;  // managed heap
  // cell_t *pin;  // temporary objects (for gc)
};

void syS_init(SchemeY *s);
void syS_shutdown(SchemeY *s);

// cell_t *syS_addfn(SchemeY *s, cell_t *var, ffun *fn);

cell_t *syS_lookup(SchemeY *s, cell_t *var);
cell_t *syS_intern(SchemeY *s, char *sym);

cell_t *syS_eval(SchemeY *s, cell_t *expr);

#endif