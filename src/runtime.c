#include "runtime.h"
#include "heap.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

SyState *SyState_new() {
  SyState *s = err_malloc(sizeof(SyState));

  s->stack = err_calloc(STACK_SIZE, sizeof(Object));
  s->top = 0;
  s->stack_size = STACK_SIZE;

  s->globals = err_calloc(GLOBALS_SIZE, sizeof(Object));
  s->globals_count = 0;
  s->globals_size = GLOBALS_SIZE;

  s->h = Heap_new(s);
  
  s->r.buffer = NULL;
  
  s->err = E_OK;
  return s;
}

void SyState_free(SyState *s) {
  free(s->stack);
  Heap_free(s->h);
  free(s->r.buffer);
  free(s);
}

Object *SyState_push(SyState *s) {
  if (s->top >= s->stack_size) {
    s->stack_size *= 2;
    s->stack = err_realloc(s->stack, s->stack_size);
  }
  return &s->stack[s->top++];
}

void SyState_pop(SyState *s) {
  if (s->top)
    s->top--;
}

// TODO: globals resize + heap copy

// Find/store a symbol, returning its binding address (list).
static Object *SyState_intern_entry(SyState *s, const char *sym) {
  Object *globals = s->globals;
  size_t size = s->globals_size;
  unsigned long i = hash(sym) % size;
  for (; car(globals + i); i = (i + 1) % size) {
    if (strcmp(sym, as(car(globals + i)).string) == 0)
      return globals + i;
  }

  Object *x = Heap_object(s->h);
  type(x) = T_SYMBOL;
  as(x).string = Heap_strdup(s->h, sym);
  car(globals + i) = x;

  return globals + i;
}

// Find/store a symbol and return the resultant object representation.
Object *SyState_intern(SyState *s, const char *sym) {
  Object *x = SyState_intern_entry(s, sym);
  return car(x);
}

// Lookup a given symbol by address and return its entry/NULL if not found.
Object *SyState_lookup(SyState *s, Object *sym) {
  Object *globals = s->globals;
  size_t size = s->globals_size;
  unsigned long i = hash(as(sym).string) % size;
  for (; car(globals + i); i = (i + 1) % size) {
    if (car(globals + i) == sym)
      return globals + i;
  }
  return NULL;
}

// Bind a value to a given symbol.
// TODO: stack operation?
int SyState_bind(SyState *s, Object *sym, Object *val) {
  Object *x = SyState_lookup(s, sym);
  if (!x)
    return E_RUNTIME;
  
  cdr(x) = val;
  return E_OK;
}

void SyState_intern_bind(SyState *s, const char *sym, Object *val) {
  Object *x = SyState_intern_entry(s, sym);
  cdr(x) = val; 
}
