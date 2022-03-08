#include "object.h"
#include "state.h"
#include "mem.h"

cell_t *cons(SchemeY *s, cell_t *_car, cell_t *_cdr) {
  cell_t *c = syM_alloc(s);
  set_cons(c, _car, _cdr);
  return c;
}

cell_t *mk_int(SchemeY *s, long i) {
  cell_t *c = syM_alloc(s);
  set_int(c, i);
  return c;
}

cell_t *mk_real(SchemeY *s, float r) {
  cell_t *c = syM_alloc(s);
  set_real(c, r);
  return c;
}

cell_t *mk_char(SchemeY *s, int ch) {
  cell_t *c = syM_alloc(s);
  set_char(c, ch);
  return c;
}

cell_t *mk_string(SchemeY *s, char *str) {
  cell_t *c = syM_alloc(s);
  set_string(c, syM_strdup(s, str));
  return c;
}

cell_t *mk_symbol(SchemeY *s, char *sym) {
  cell_t *c = syM_alloc(s);
  set_symbol(c, syM_strdup(s, sym));
  return c;
}

cell_t *mk_ffun(SchemeY *s, ffun_t *f) {
  cell_t *c = syM_alloc(s);
  set_ffun(c, f);
  return c;
}

vector_t *mk_vector_t(SchemeY *s, size_t sz) {
  vector_t *v = syM_malloc(s, sizeof(vector_t));
  v->_items = syM_calloc(s, sz, sizeof(cell_t));
  v->_len = 0;
  v->_size = sz;
  return v;
}

cell_t *mk_vector(SchemeY *s, size_t sz) {
  cell_t *c = syM_alloc(s);
  set_vector(c, mk_vector_t(s, sz));
  return c;
}

cell_t *mk_table(SchemeY *s, size_t sz) {
  cell_t *c = syM_alloc(s);
  set_table(c, mk_vector_t(s, sz));
  return c;
}

cell_t *mk_port(SchemeY *s, FILE *p) {
  cell_t *c = syM_alloc(s);
  set_port(c, p);
  return c; 
}
