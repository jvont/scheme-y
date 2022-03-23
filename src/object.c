#include "object.h"
#include "mem.h"

#include <string.h>

Object *cons(Object *_car, Object *_cdr) {
  Object *x = mem_malloc(sizeof(Object));
  car(x) = _car;
  cdr(x) = _cdr;
  return (Object *)tag(x);
}

Object *integer(long i) {
  Object *x = mem_malloc(sizeof(Object));
  type(x) = T_INTEGER;
  as(x).integer = i;
  return x;
}

Object *real(float r) {
  Object *x = mem_malloc(sizeof(Object));
  type(x) = T_REAL;
  as(x).real = r;
  return x;
}

Object *character(int c) {
  Object *x = mem_malloc(sizeof(Object));
  type(x) = T_CHARACTER;
  as(x).character = c;
  return x;
}

Object *string(const char *s) {
  Object *x = mem_malloc(sizeof(Object));
  type(x) = T_STRING;
  as(x).string = mem_strdup(s);
  return x;
}

Object *symbol(const char *s) {
  Object *x = mem_malloc(sizeof(Object));
  type(x) = T_SYMBOL;
  as(x).string = mem_strdup(s);
  return x;
}

Object *ffun(ffun_t *f) {
  Object *x = mem_malloc(sizeof(Object));
  type(x) = T_FFUN;
  as(x).ffun = f;
  return x;
}

static Vector *vvector(size_t n) {
  Vector *v = mem_calloc(n + b2o(sizeof(Vector)), sizeof(Object));
  v->len = 0;
  v->size = n;
  return v;
}

Object *vector(size_t n) {
  Object *x = mem_malloc(sizeof(Object));
  type(x) = T_VECTOR;
  as(x).vector = vvector(n);
  return x;
}

Object *table(size_t n) {
  Object *x = mem_malloc(sizeof(Object));
  type(x) = T_TABLE;
  as(x).vector = vvector(n);
  return x;
}

Object *port(FILE *p) {
  Object *x = mem_malloc(sizeof(Object));
  type(x) = T_PORT;
  as(x).port = p;
  return x;
}
