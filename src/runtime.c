#include "runtime.h"
#include "heap.h"
#include "utils.h"

#include <stdlib.h>

SyState *SyState_new() {
  SyState *s = err_malloc(sizeof(SyState));

  s->stack = err_calloc(STACK_MIN, sizeof(Object));
  s->top = 0;
  s->stack_size = STACK_MIN;

  return s;
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

void SyState_free(SyState *s) {
  free(s->stack);

  free(s);
}
