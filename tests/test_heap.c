/*
** Testing object allocation and garbage collection.
*/
#include "testlib.h"
#include "../src/heap.h"
#include "../src/runtime.h"

#include <stdlib.h>
#include <string.h>

static SyState *s;  // TODO: use real state

int test_malloc() {
  Heap *h = Heap_new(s);
  Object *x = Heap_malloc(h, sizeof(Object));
  size_t n = Heap_count(h);
  Heap_free(h);
  return n == 1 ? TEST_PASS : TEST_FAIL;
}

int test_calloc() {
  Heap *h = Heap_new(s);
  Object *x = Heap_calloc(h, 2, sizeof(Object));
  int cmp = memcmp(x, x + 1, sizeof(Object));
  Heap_free(h);
  return cmp == 0 ? TEST_PASS : TEST_FAIL;
}

// int test_gc_free() {
//   Heap *h = Heap_new(s);
//   Object *x = Heap_malloc(h, sizeof(Object));
//   type(x) = T_INTEGER;
//   as(x).integer = 1;

//   Heap_collect(h);
//   size_t n = Heap_count(h);
//   Heap_free(h);
//   return n == 0 ? TEST_PASS : TEST_FAIL;
// }

// int test_gc_root() {
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

// int test_root_pop() {
//   Heap *h = Heap_new(s);
//   Object *x = Heap_malloc(h, sizeof(Object));
//   type(x) = T_INTEGER;
//   as(x).integer = 1;

//   mem_push(&x);
//   mem_pop();
//   garbage_collect(0);
//   size_t n = Heap_count(h);
//   Heap_free(h);
//   return n == 0 ? TEST_PASS : TEST_FAIL;
// }

// int test_gc_list() {
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

// int test_gc_cyclic() {
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
  { test_malloc, "Heap_malloc" },
  { test_calloc, "Heap_calloc" },
  // { test_gc_free, "collect unrooted" },
  // { test_gc_root, "collect root" },
  // { test_root_pop, "pop root" },
  // { test_gc_list, "collect list root" },
  // { test_gc_cyclic, "collect cyclic list" }
};

int main() {
  run_tests(table, "test heap.h functions");
  return 0;
}
