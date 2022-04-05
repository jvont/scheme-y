/*
** Bytecode compiler.
*/
#ifndef COMPILER_H
#define COMPILER_H

#include "object.h"

// An instruction consists of an opcode and argument.
typedef struct Instruction {
  enum {
    OP_CONST,
    OP_LOAD,
    OP_STORE,
    OP_DEF,
    OF_FUN,
    OP_POP,
    OP_JUMP,
    OP_RETURN,
    OP_CALL
  } op;
  unsigned int arg;
} Instruction;

// TODO: function registry (allow redefinition of builtin keywords)

typedef struct Closure {

  Instruction *code;
  Object *constants;  // array of constants
  
  // TODO: figure out how variable lookup is handled

  Object *env;  // associated environment

} Closure;

// typedef struct Compiler {

// } Compiler;

// Compiler *Compiler_new();
// void      Compiler_free(Compiler *c);

// Closure *Compiler_compile(SyState *s);

#endif