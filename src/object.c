#include "object.h"
#include "state.h"
#include "mem.h"

#include <string.h>

cell *cons(SchemeY *s, cell *a, cell *d) {
  cell *c = obj_alloc(s);
  set_cons(c, a, d);
  return c;
}

cell *mk_int(SchemeY *s, long i) {
  cell *c = obj_alloc(s);
  set_int(c, i);
  return c;
}

cell *mk_real(SchemeY *s, float r) {
  cell *c = obj_alloc(s);
  set_real(c, r);
  return c;
}

cell *mk_char(SchemeY *s, int ch) {
  cell *c = obj_alloc(s);
  set_char(c, ch);
  return c;
}

cell *mk_string(SchemeY *s, char *str) {
  size_t n = sizeof(size_t) + strlen(str) + 1;
  cell *c = heap_malloc(s, n);
  type(c) = T_STRING;
  strcpy(as(c).string, str);
  return c;
}

cell *mk_symbol(SchemeY *s, char *sym) {
  cell *c = mk_string(s, sym);
  type(c) = T_SYMBOL;
  return c;
}

cell *mk_ffun(SchemeY *s, ffun_t *f) {
  cell *c = obj_alloc(s);
  set_ffun(c, f);
  return c;
}

cell *mk_vector(SchemeY *s, size_t sz) {
  cell *c = obj_alloc(s);
  vector *v = mk_vector_t(s, sz);
  set_vector(c, v);
  return c;
}

cell *mk_table(SchemeY *s, size_t sz) {
  cell *c = obj_alloc(s);
  vector *v = mk_vector_t(s, sz);
  set_table(c, v);
  return c;
}

cell *mk_port(SchemeY *s, FILE *p) {
  cell *c = obj_alloc(s);
  set_port(c, p);
  return c;
}

vector *mk_vector_t(SchemeY *s, size_t sz) {
  size_t vs = cellsize(sizeof(vector));
  vector *v = heap_calloc(s, sz + vs, sizeof(cell));
  v->len = 0;
  v->size = sz;
  return v;
}
