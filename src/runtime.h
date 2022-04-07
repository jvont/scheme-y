/*
** Interpreter state.
*/
#ifndef STATE_H
#define STATE_H

#include "object.h"

#include <stddef.h>

#define STACK_SIZE 16
#define GLOBALS_SIZE 128

#define DEFAULT_INPUT_PORT stdin
#define DEFAULT_OUTPUT_PORT stdout

// Scheme-y error codes.
enum {
  E_OK,
  E_SYNTAX,  // syntax error
  E_RUNTIME,  // runtime error
  E_MEMORY  // out of memory, called on exit
};

// Scheme object parser. Used for reading from streams.
typedef struct Reader {
  FILE *port;  // current input port
  char *buffer;  // token buffer
  size_t pos, size;
  int ch;  // next character
  size_t depth, lineno;  // read position
  int prompt;  // interactive prompt?
  SyState *s;  // associated state
} Reader;

// Scheme-y interpreter state.
struct SyState {
  Object *stack;
  size_t top, stack_size;

  Object *globals;  // hash table of top-level bindings
  size_t globals_count, globals_size;

  Reader r;
  int err;  // error status
  struct Heap *h;
};

SyState *SyState_new();
void     SyState_free(SyState *s);

Object  *SyState_push(SyState *s);
void     SyState_pop(SyState *s);

Object  *SyState_intern(SyState *s, const char *sym);
Object  *SyState_lookup(SyState *s, Object *sym);
int      SyState_bind(SyState *s, Object *sym, Object *val);
void     SyState_intern_bind(SyState *s, const char *sym, Object *val);

// void     SyState_run(SyState *s);

#endif