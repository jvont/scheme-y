#include "object.h"
#include "state.h"
#include "mem.h"

cell_t *cons(SchemeY *s, cell_t *a, cell_t *d) {
  cell_t *c = obj_alloc(s);
  set_cons(c, a, d);
  return c;
}

cell_t *mk_int(SchemeY *s, long i) {
  cell_t *c = obj_alloc(s);
  set_int(c, i);
  return c;
}

cell_t *mk_real(SchemeY *s, float r) {
  cell_t *c = obj_alloc(s);
  set_real(c, r);
  return c;
}

cell_t *mk_char(SchemeY *s, int ch) {
  cell_t *c = obj_alloc(s);
  set_char(c, ch);
  return c;
}

cell_t *mk_string(SchemeY *s, char *str) {
  cell_t *c = obj_alloc(s);
  char *dup = heap_strdup(s, str);
  set_string(c, dup);
  return c;
}

cell_t *mk_symbol(SchemeY *s, char *sym) {
  cell_t *c = obj_alloc(s);
  char *dup = heap_strdup(s, sym);
  set_symbol(c, dup);
  return c;
}

cell_t *mk_ffun(SchemeY *s, ffun_t *f) {
  cell_t *c = obj_alloc(s);
  set_ffun(c, f);
  return c;
}

cell_t *mk_vector(SchemeY *s, size_t sz) {
  cell_t *c = obj_alloc(s);
  vector_t *v = mk_vector_t(s, sz);
  set_vector(c, v);
  return c;
}

cell_t *mk_table(SchemeY *s, size_t sz) {
  cell_t *c = obj_alloc(s);
  vector_t *v = mk_vector_t(s, sz);
  set_table(c, v);
  return c;
}

cell_t *mk_port(SchemeY *s, FILE *p) {
  cell_t *c = obj_alloc(s);
  set_port(c, p);
  return c;
}

vector_t *mk_vector_t(SchemeY *s, size_t sz) {
  vector_t *v = heap_malloc(s, sizeof(vector_t));
  v->items = heap_calloc(s, sz, sizeof(cell_t));
  v->len = 0;
  v->size = sz;
  return v;
}
