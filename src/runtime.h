/*
** Interpreter state.
**
** Each thread state object stores environment frames, a run queue (code), and
** evaluation stack. Each of these structures are added as roots to the
** managed heap. The goal of this layout is to facilitate concurrent execution
** in the future.
**
** Environments are pairs, where car points to current bindings and cdr points
** to the next environment pair to search for a specific interned symbol. The
** current environment is added as a root for collection. The top-level
** environment (env_tail) is implemented as a hash table and shared between
** threads.
*/
#ifndef STATE_H
#define STATE_H

#include "object.h"

#include <stddef.h>

#define GLOBAL_ENV_SIZE 128  // global execution frame size

#define DEFAULT_INPUT_PORT stdin
#define DEFAULT_OUTPUT_PORT stdout

// VM Opcodes
typedef enum OpCode {
  OP_CONST,
  OP_LOAD,
  OP_STORE,
  OP_DEF,
  OF_FUN,
  OP_POP,
  OP_JUMP,
  OP_RETURN,
  OP_CALL
} OpCode;

// opcode, arity, behaviour
typedef struct Instruction {
  OpCode code;
  // Object *arg;
} Instruction;

// state stores VM info (instructions, ip/pc, stack, heap, etc.)

struct SyState {
  Object *stack, *top;

  

  int err;  // error status
  struct Heap *h;
};

SyState *SyState_new();
void SyState_free(SyState *s);


// void SyState_run(SyState *s);

// Object *lookup(Object *env);
// Object *bind(Object *env, Object *var, Object *val);
// Object *intern(Object *env, const char *s);

#endif