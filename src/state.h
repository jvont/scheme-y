/*
** Interpreter state.
*/
#ifndef _SY_STATE_H
#define _SY_STATE_H

#include "object.h"
#include "mem.h"

#define GLOBAL_ENV_SIZE 128
#define SYMBOLS_SIZE 128
#define HEAP_SIZE 1024

struct SchemeY {
  cell *global_env;  // top-level environment
  cell *symbols;  // interned symbols

  cell *env;  // current environment

  cell *input_port;  // current input port
  cell *output_port;  // current output port

  Heap *heap;  // heap
  // cell *pin;  // temporary objects (for gc)
};

void syS_init(SchemeY *s);
void syS_shutdown(SchemeY *s);

cell *syS_lookup(SchemeY *s, cell *var);
cell *syS_intern(SchemeY *s, char *sym);

// garbage collection
// evaluation

#endif