/*
** Testing object allocation and garbage collection.
*/
#include "testlib.h"
#include "../src/heap.h"
#include "../src/runtime.h"

#include <stdlib.h>
#include <string.h>

static SyState *s;  // TODO: use real state

int test_object() {
  Heap *h = Heap_new(s);
  Object *x = Heap_object(h);
  size_t n = Heap_count(h);

  Heap_free(h);
  return n == sizeof(Object) ? TEST_PASS : TEST_FAIL;
}

int test_malloc() {
  Heap *h = Heap_new(s);
  Object *x = Heap_malloc(h, 2 * sizeof(Object));
  size_t n = Heap_count(h);
  Heap_free(h);
  return n == 2 * sizeof(Object) ? TEST_PASS : TEST_FAIL;
}

int test_strdup() {
  Heap *h = Heap_new(s);
  char *src = "test";
  char *dest = Heap_strdup(h, src);
  int cmp = strcmp(src, dest);
  Heap_free(h);
  return cmp == 0 ? TEST_PASS : TEST_FAIL;
}

int test_collect_all() {
  Heap *h = Heap_new(s);
  Object *x = Heap_object(h);
  Heap_collect(h);
  size_t n = Heap_count(h);
  Heap_free(h);
  return n == 0 ? TEST_PASS : TEST_FAIL;
}

// int test_collect_root() {
//   Heap *h = Heap_new(s);
//   Object *x = Heap_malloc(h, sizeof(Object));
//   type(x) = T_INTEGER;
//   as(x).integer = 1;

//   mem_push(&x);
//   garbage_collect(0);
//   size_t n = Heap_count(h);
//   Heap_free(h);
//   return n == 1 ? TEST_PASS : TEST_FAIL;
// }

// int test_collect_list() {
//   Heap *h = Heap_new(s);
//   Object *_car = Heap_malloc(h, sizeof(Object));
//   type(_car) = T_INTEGER;
//   as(_car).integer = 1;

//   Object *x = Heap_malloc(h, sizeof(Object));
//   car(x) = _car;
//   cdr(x) = NULL;
//   x = (Object *)taglist(x);

//   mem_push(&x);
//   garbage_collect(0);
//   size_t n = Heap_count(h);
//   Heap_free(h);
//   return n == 2 ? TEST_PASS : TEST_FAIL;
// }

// int test_collect_cyclic() {
//   Heap *h = Heap_new(s);
//   Object *x = Heap_malloc(h, sizeof(Object));
//   Object *y = Heap_malloc(h, sizeof(Object));
//   car(x) = NULL; cdr(x) = y;
//   car(y) = NULL; cdr(y) = x;
//   size_t n = Heap_count(h);
//   Heap_free(h);
//   return n == 2 ? TEST_PASS : TEST_FAIL;
// }

Test table[] = {
  { test_object, "allocate object" },
  { test_malloc, "allocate data" },
  { test_strdup, "duplicate string data" },
  { test_collect_all, "collect without roots" },
  // { test_collect_root, "collect roots" },
  // { test_collect_list, "collect list root" },
  // { test_collect_cyclic, "collect cyclic list" }
};

int main() {
  s = malloc(sizeof(SyState));
  s->top = 0;

  run_tests(table, "managed heap functions");

  free(s);
  return 0;
}
