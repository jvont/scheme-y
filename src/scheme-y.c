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

int main(int argc, char **argv) {

  SyState *s = Sy_open();
  Heap *h = s->h;

  Object *x = Heap_malloc(h, sizeof(Object));
  type(x) = T_INTEGER;
  as(x).integer = 5;

  s->stack = err_malloc(sizeof(Object));
  car(s->stack) = x;
  cdr(s->stack) = NULL;
  s->stack = (Object *)tag(s->stack);

  List *root = (List *)untag(s->stack);

  Heap_collect(h);  // move to old space
  Heap_collect(h);

  Sy_close(s);

  return 0;
}
