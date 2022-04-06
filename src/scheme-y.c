#include "scheme-y.h"
#include "runtime.h"
#include "heap.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

SyState *sy_open() {
  SyState *s = SyState_new();

  
  return s;
}

void sy_close(SyState *s) {
  Heap_free(s->h);

  free(s->buffer);

  SyState_free(s);
}

void sy_pushnil(SyState *s) {
  Object *x = SyState_push(s);
  type(x) = T_NIL;
  as(x).integer = 0;
}

void sy_pushlist(SyState *s, Object *ca, Object *cd) {
  Object *x = SyState_push(s);
  car(x) = ca;
  cdr(x) = cd;
}

void sy_pushinteger(SyState *s, int32_t i) {
  Object *x = SyState_push(s);
  type(x) = T_INTEGER;
  as(x).integer = i;
}

void sy_pushreal(SyState *s, float r) {
  Object *x = SyState_push(s);
  type(x) = T_REAL;
  as(x).real = r;
}

void sy_pushboolean(SyState *s, int b) {
  Object *x = SyState_push(s);
  type(x) = T_BOOLEAN;
  as(x).integer = b;
}

void sy_pushcharacter(SyState *s, uint32_t ch) {
  Object *x = SyState_push(s);
  type(x) = T_CHARACTER;
  as(x).character = ch;
}

void sy_pushstring(SyState *s, const char *str) {
  Object *x = SyState_push(s);
  type(x) = T_STRING;
  as(x).string = Heap_strdup(s->h, str);
}

void sy_pushnstring(SyState *s, const char *str, size_t n) {
  char *dest = Heap_malloc(s->h, n + 1);
  strncpy(dest, str, n);
  dest[n] = '\0';
  
  Object *x = SyState_push(s);
  type(x) = T_STRING;
  as(x).string = dest;
}

void sy_pushffunction(SyState *s, FFunction *f) {
  Object *x = SyState_push(s);
  type(x) = T_FFUNCTION;
  as(x).ffunction = f;
}

void sy_pushvector(SyState *s, size_t size) {
  Vector *v = Heap_malloc(s->h, sizeof(Vector) + size * sizeof(Object));
  for (size_t i = 0; i < size; i++) {
    type(&v->items[i]) = T_NIL;
  }
  v->len = 0;
  v->size = size;

  Object *x = SyState_push(s);
  type(x) = T_VECTOR;
  as(x).vector = v;
}

void sy_pushport(SyState *s, FILE *p) {
  Object *x = SyState_push(s);
  type(x) = T_CHARACTER;
  as(x).port = p;
}

void sy_intern(SyState *s, const char *sym) {
  // TODO: DO
}

void sy_nintern(SyState *s, const char *sym, size_t n) {
  // TODO: DO
}

