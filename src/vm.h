/*
** Register-based virtual machine
*/
#ifndef _VM_H
#define _VM_H

enum {
  OP_CONST,
  OP_LOAD,
  OP_STORE,
  OP_DEFINE,
  OP_FUNCTION,
  OP_POP,
  OP_JUMP,
  OP_FJUMP,
  OP_RETURN,
  OP_CALL
};

#endif