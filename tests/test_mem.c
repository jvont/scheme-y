/*
** Testing object allocation and garbage collection.
*/

#include "testlib.h"
#include "../src/mem.h"

#include <string.h>

int test_malloc() {
  mem_init(2);
  Object *x = mem_malloc(sizeof(Object));
  size_t sz = heap_size();
  mem_shutdown();
  return (sz == 1) ? TEST_PASS : TEST_FAIL;
}

int test_calloc() {
  mem_init(2);
  Object *x = mem_calloc(2, sizeof(Object));
  int cmp = memcmp(x, x + 1, sizeof(Object));
  mem_shutdown();
  return (cmp == 0) ? TEST_PASS : TEST_FAIL;
}

int test_gc_free() {
  mem_init(2);
  Object *x = mem_malloc(sizeof(Object));
  type(x) = T_INTEGER;
  as(x).integer = 1;

  gc();
  size_t sz = heap_size();
  mem_shutdown();
  return (sz == 0) ? TEST_PASS : TEST_FAIL;
}

int test_gc_root() {
  mem_init(2);
  Object *x = mem_malloc(sizeof(Object));
  type(x) = T_INTEGER;
  as(x).integer = 1;

  mem_root(&x);
  gc();
  size_t sz = heap_size();
  mem_shutdown();
  return (sz == 1) ? TEST_PASS : TEST_FAIL;
}

int test_gc_list() {  
  mem_init(2);

  Object *_car = mem_malloc(sizeof(Object));
  type(_car) = T_INTEGER;
  as(_car).integer = 1;

  Object *x = mem_malloc(sizeof(Object));
  car(x) = _car;
  cdr(x) = NULL;
  x = (Object *)tag(x);

  mem_root(&x);
  gc();
  size_t sz = heap_size();
  mem_shutdown();
  return (sz == 2) ? TEST_PASS : TEST_FAIL;
}

Test table[] = {
  { test_malloc, "mem_malloc" },
  { test_calloc, "mem_calloc" },
  { test_gc_free, "collect unrooted" },
  { test_gc_root, "collect root" },
  { test_gc_list, "collect list root" }
};

int main() {
  run_tests(table, "test mem.h functions");

  return 0;
}
