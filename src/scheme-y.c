#include "scheme-y.h"
#include "runtime.h"
#include "heap.h"
#include "utils.h"

#include <stdlib.h>

SyState *Sy_open() {
  SyState *s = SyState_new();
  s->h = Heap_new(s);

  s->top = 0;
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

//  STACK        CONS
// [x, y]      [c]->[y]
// [x, y, c]    |->[x]
static void Sy_cons(SyState *s) {
  Object *c = &s->stack[s->top++];
  car(c) = &s->stack[s->top - 3];
  cdr(c) = &s->stack[s->top - 2];
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

  
  Sy_close(s);

  return 0;
}
