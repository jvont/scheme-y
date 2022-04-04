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
  Object *x = SyState_push(s);
  type(x) = T_INTEGER;
  as(x).integer = i;
}

static void Sy_pushstring(SyState *s, const char *str) {
  Object *x = SyState_push(s);
  type(x) = T_STRING;
  as(x).string = Heap_strdup(s->h, str);
}

static void Sy_cons(SyState *s) {
  Object *x = SyState_push(s);
  car(x) = &s->stack[s->top - 3];
  cdr(x) = &s->stack[s->top - 2];
}

static void Sy_pushvector(SyState *s, size_t size) {
  size_t n = objsize(sizeof(Vector)) + size;
  Vector *v = Heap_calloc(s->h, n, sizeof(Object));
  v->len = 1;
  v->size = size;

  Object *x = &s->stack[s->top++];
  type(x) = T_VECTOR;
  as(x).vector = v;
}

int main(int argc, char **argv) {
  SyState *s = Sy_open();
  Heap *h = s->h;

  Object *a = Heap_object(h);
  type(a) = T_INTEGER;
  as(a).integer = 0;
  Object *x = SyState_push(s);
  car(x) = a;
  cdr(x) = NULL;

  Heap_collect(h);
  printf("%zu\n", Heap_count(h));
  printf("Location: %d\n", Heap_location(h, car(x)));
  Heap_collect(h);
  printf("Location: %d\n", Heap_location(h, car(x)));
  Heap_collect(h);
  printf("Location: %d\n", Heap_location(h, car(x)));
  Heap_collect(h);
  printf("Location: %d\n", Heap_location(h, car(x)));
  Heap_collect(h);
  printf("Location: %d\n", Heap_location(h, car(x)));
  Heap_collect(h);
  printf("Location: %d\n", Heap_location(h, car(x)));

  Sy_close(s);

  return 0;
}
