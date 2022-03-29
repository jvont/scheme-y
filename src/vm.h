/*
**
*/
#ifndef VM_H
#define VM_H

#include "object.h"

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

void State_run(struct State *s);

#endif