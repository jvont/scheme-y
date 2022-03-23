#include "../src/mem.h"

#include <assert.h>
#include <stdio.h>

int main() {
  Object *x, *_car, *_cdr;
  size_t s1, s2, s3;
  
  mem_init(2);

  _car = mem_malloc(sizeof(Object));
  type(_car) = T_INTEGER;
  as(_car).integer = 1;

  _cdr = mem_malloc(sizeof(Object));
  type(_cdr) = T_INTEGER;
  as(_cdr).integer = 2;

  x = mem_malloc(sizeof(Object));
  car(x) = _car;
  cdr(x) = _cdr;
  x = (Object *)tag(x);

  mem_root(&x);

  /* Collect none */
  s1 = heap_size();
  gc();
  s2 = heap_size();
  assert(s2 == s1);

  /* Collect _cdr */
  cdr(x) = NULL;
  gc();
  s3 = heap_size();
  assert(s3 == (s2 - 1));

  mem_shutdown();
  return 0;
}