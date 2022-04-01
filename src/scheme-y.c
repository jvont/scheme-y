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
  Object *x = &s->stack[s->top++];
  type(x) = T_INTEGER;
  as(x).integer = i;
}

// 1. [x, y] <- stack   2.  [z]->[y] <- cdr
//                           |->[x]  <- car
static void Sy_cons(SyState *s) {
  Object *d = Heap_object(s->h);
  *d = s->stack[s->top - 2];

  Object *x = &s->stack[s->top - 2];
  cdr(x) = d;

  Object *a = Heap_object(s->h);
  *a = s->stack[s->top - 1];
  car(x) = a;

  s->top--;
}

static void Sy_pushvector(SyState *s, size_t size) {
  size_t n = objsize(sizeof(Vector)) + size;
  Vector *v = Heap_calloc(s->h, n, sizeof(Object));
  v->len = 1;
  v->size = size;

  v->items[0] = s->stack[1];

  Object *x = &s->stack[s->top++];
  type(x) = T_VECTOR;
  as(x).vector = v;
}

int main(int argc, char **argv) {
  SyState *s = Sy_open();
  Heap *h = s->h;

  s->top = 0;
  Sy_pushint(s, 1);
  Sy_pushint(s, 2);
  Sy_pushint(s, 3);
  Sy_cons(s);
  Sy_pushvector(s, 5);

  printf("%zu\n", Heap_count(h));
  Heap_collect(h);
  printf("%zu\n", Heap_count(h));
  Heap_object(h);
  printf("%zu\n", Heap_count(h));

  Sy_close(s);

  return 0;
}
