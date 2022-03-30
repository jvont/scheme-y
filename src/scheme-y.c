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
  if (s->top < 10) {
    Object *x = Heap_malloc(s->h, sizeof(Object));
    type(x) = T_INTEGER;
    as(x).integer = i;
    s->stack[s->top++] = x;
  }
}

int main(int argc, char **argv) {

  SyState *s = Sy_open();
  Heap *h = s->h;

  s->top = 0;
  Sy_pushint(s, 1);
  Sy_pushint(s, 2);
  Sy_pushint(s, 3);
  Sy_pushint(s, 4);
  Sy_pushint(s, 5);

  Heap_collect(h);
  Heap_collect(h);
  Heap_collect(h);
  Heap_collect(h);
  Heap_collect(h);

  Sy_close(s);

  return 0;
}
