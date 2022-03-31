#include "scheme-y.h"
#include "runtime.h"
#include "heap.h"
#include "utils.h"

#include <stdlib.h>

SyState *Sy_open() {
  SyState *s = SyState_new();
  s->h = Heap_new(s);
  return s;
}

void Sy_close(SyState *s) {
  Heap_free(s->h);
  SyState_free(s);
}

static void Sy_pushint(SyState *s, int32_t i) {
  Object *x = Heap_object(s->h);
  type(x) = T_INTEGER;
  as(x).integer = i;
  s->stack[s->top++] = x;
}

static void Sy_cons(SyState *s) {
  Object *x = Heap_object(s->h);
  car(x) = s->stack[--s->top];
  cdr(x) = s->stack[--s->top];
  s->stack[s->top + 1] = NULL;
  s->stack[s->top++] = x;
}

int main(int argc, char **argv) {

  sizeof(Object);

  SyState *s = Sy_open();
  Heap *h = s->h;

  s->top = 0;
  Sy_pushint(s, 1);
  Sy_pushint(s, 2);
  Sy_pushint(s, 3);
  Sy_cons(s);

  Heap_collect(h);
  Heap_collect(h);
  Heap_collect(h);

  Sy_close(s);

  return 0;
}
