#include "scheme-y.h"
#include "runtime.h"
#include "heap.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

SyState *Sy_open() {
  SyState *s = SyState_new();
  s->h = Heap_new(s);

  s->prompt = 1;

  s->buffer = NULL;

  s->top = 0;
  return s;
}

void Sy_close(SyState *s) {
  Heap_free(s->h);

  free(s->buffer);

  SyState_free(s);
}

void Sy_pushlist(SyState *s, Object *ca, Object *cd) {
  Object *x = SyState_push(s);
  car(x) = ca;
  cdr(x) = cd;
}

void Sy_pushinteger(SyState *s, int32_t i) {
  Object *x = SyState_push(s);
  type(x) = T_INTEGER;
  as(x).integer = i;
}

void Sy_pushreal(SyState *s, float r) {
  Object *x = SyState_push(s);
  type(x) = T_REAL;
  as(x).real = r;
}

void Sy_pushcharacter(SyState *s, uint32_t ch) {
  Object *x = SyState_push(s);
  type(x) = T_CHARACTER;
  as(x).character = ch;
}

void Sy_pushstring(SyState *s, const char *str) {
  Object *x = SyState_push(s);
  type(x) = T_STRING;
  as(x).string = Heap_strdup(s->h, str);
}

void Sy_pushnstring(SyState *s, const char *str, size_t n) {
  char *dest = Heap_malloc(s->h, n + 1);
  strncpy(dest, str, n);
  dest[n] = '\0';
  
  Object *x = SyState_push(s);
  type(x) = T_STRING;
  as(x).string = dest;
}

void Sy_pushffunction(SyState *s, FFunction *f) {
  Object *x = SyState_push(s);
  type(x) = T_FFUNCTION;
  as(x).ffunction = f;
}

void Sy_pushvector(SyState *s, size_t size) {
  size_t n = objsize(sizeof(Vector)) + size;
  Vector *v = Heap_calloc(s->h, n, sizeof(Object));
  v->len = 1;
  v->size = 0;

  Object *x = SyState_push(s);
  type(x) = T_VECTOR;
  as(x).vector = v;
}

void Sy_pushport(SyState *s, FILE *p) {
  Object *x = SyState_push(s);
  type(x) = T_CHARACTER;
  as(x).port = p;
}

// void Sy_cons(SyState *s) {
//   Object *x = SyState_push(s);
//   car(x) = &s->stack[s->top - 3];
//   cdr(x) = &s->stack[s->top - 2];
// }

// int main(int argc, char **argv) {
//   SyState *s = Sy_open();
//   Heap *h = s->h;

//   Object *a = Heap_object(h);
//   type(a) = T_INTEGER;
//   as(a).integer = 0;
//   Object *x = SyState_push(s);
//   car(x) = a;
//   cdr(x) = NULL;

//   Heap_collect(h);
//   printf("%zu\n", Heap_count(h));
//   printf("Location: %d\n", Heap_location(h, car(x)));
//   Heap_collect(h);
//   printf("Location: %d\n", Heap_location(h, car(x)));
//   Heap_collect(h);
//   printf("Location: %d\n", Heap_location(h, car(x)));
//   Heap_collect(h);
//   printf("Location: %d\n", Heap_location(h, car(x)));
//   Heap_collect(h);
//   printf("Location: %d\n", Heap_location(h, car(x)));
//   Heap_collect(h);
//   printf("Location: %d\n", Heap_location(h, car(x)));

//   Sy_close(s);

//   return 0;
// }

#include "parser.h"

int main(int argc, char **argv) {
  SyState *s = Sy_open();

  Parser_parse(s);

  Sy_close(s);

  return 0;
}
