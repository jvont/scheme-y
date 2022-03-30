/*
** Testing object allocation and garbage collection.
*/
#include "testlib.h"
#include "../src/heap.h"

#include <stdlib.h>
#include <string.h>

static Heap *h;
static Heap_clear(Heap *h) {
  for (size_t i = 0; i <= N_GENERATIONS; i++) {
    Generation *g = h->g0 + i;
    g->alloc = g->blocks;
  }
  h->gn = h->g0 + N_GENERATIONS - 1;
}

static size_t Gen_blocks(Generation *g) {
  return g->alloc - g->blocks;
}

int test_malloc() {
  Heap *h = Heap_new();
  Object *x = Heap_malloc(h, sizeof(Object));
  size_t sz = Gen_blocks(h->g0);
  Heap_clear(h);
  return sz == 1 ? TEST_PASS : TEST_FAIL;
}

int test_calloc() {
  Object *x = Heap_calloc(h, 2, sizeof(Object));
  int cmp = memcmp(x, x + 1, sizeof(Object));
  Heap_clear(h);
  return cmp == 0 ? TEST_PASS : TEST_FAIL;
}

// int test_gc_free() {
//   Object *x = Heap_malloc(h, sizeof(Object));
//   type(x) = T_INTEGER;
//   as(x).integer = 1;

//   garbage_collect(0);
//   size_t sz = Gen_blocks(h->g0);
//   Heap_clear(h);
//   return sz == 0 ? TEST_PASS : TEST_FAIL;
// }

// int test_gc_root() {
//   Object *x = Heap_malloc(h, sizeof(Object));
//   type(x) = T_INTEGER;
//   as(x).integer = 1;

//   mem_push(&x);
//   garbage_collect(0);
//   size_t sz = Gen_blocks(h->g0);
//   Heap_clear(h);
//   return sz == 1 ? TEST_PASS : TEST_FAIL;
// }

// int test_root_pop() {
//   Object *x = Heap_malloc(h, sizeof(Object));
//   type(x) = T_INTEGER;
//   as(x).integer = 1;

//   mem_push(&x);
//   mem_pop();
//   garbage_collect(0);
//   size_t sz = Gen_blocks(h->g0);
//   Heap_clear(h);
//   return sz == 0 ? TEST_PASS : TEST_FAIL;
// }

// int test_gc_list() {
//   Object *_car = Heap_malloc(h, sizeof(Object));
//   type(_car) = T_INTEGER;
//   as(_car).integer = 1;

//   Object *x = Heap_malloc(h, sizeof(Object));
//   car(x) = _car;
//   cdr(x) = NULL;
//   x = (Object *)tag(x);

//   mem_push(&x);
//   garbage_collect(0);
//   size_t sz = Gen_blocks(h->g0);
//   Heap_clear(h);
//   return sz == 2 ? TEST_PASS : TEST_FAIL;
// }

// int test_gc_cyclic() {
//   Object *x = Heap_malloc(h, sizeof(Object));
//   Object *y = Heap_malloc(h, sizeof(Object));
//   car(x) = NULL; cdr(x) = y;
//   car(y) = NULL; cdr(y) = x;
//   size_t sz = Gen_blocks(h->g0);
//   Heap_clear(h);
//   return sz == 2 ? TEST_PASS : TEST_FAIL;
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
  h = Heap_new();
  run_tests(table, "test heap.h functions");
  Heap_free(h);
  return 0;
}
