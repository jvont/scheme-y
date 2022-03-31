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
  Object *x = Heap_malloc(s->h, sizeof(Object));
  type(x) = T_INTEGER;
  as(x).integer = i;
  s->stack[s->top++] = x;
}

static Object *Sy_pushcons(SyState *s) {
  Object *x = Heap_malloc(s->h, sizeof(Object));
  x = (Object *)tag(x);
  s->stack[s->top++] = x;
  return x;
}

int main(int argc, char **argv) {

  sizeof(Object);

  SyState *s = Sy_open();
  Heap *h = s->h;

  s->top = 0;
  Sy_pushint(s, 1);
  Sy_pushint(s, 2);
  Sy_pushint(s, 3);

  Object *a = s->stack[s->top - 1];
  Object *c = Sy_pushcons(s);
  Object *d = Heap_malloc(s->h, sizeof(Object));
  type(d) = T_REAL;
  as(d).real = 3.14;
  car(c) = a;
  cdr(c) = d;

  Heap_collect(h);
  Heap_collect(h);
  Heap_collect(h);

  Sy_close(s);

  return 0;
}
