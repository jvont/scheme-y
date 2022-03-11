#include "object.h"
#include "state.h"
#include "mem.h"

cell_t *set_cons(cell_t *c, cell_t *a, cell_t *d) { car(c) = a; cdr(c) = d; return c; }
cell_t *set_int(cell_t *c, long i) { type(c) = T_INT; as(c).integer = i; return c; }
cell_t *set_real(cell_t *c, float r) { type(c) = T_REAL; as(c).real = r; return c; }
cell_t *set_char(cell_t *c, int ch) { type(c) = T_CHAR; as(c).character = ch; return c; }
cell_t *set_string(cell_t *c, char *str) { type(c) = T_STRING; as(c).string = str; return c; }
cell_t *set_symbol(cell_t *c, char *sym) { type(c) = T_SYMBOL; as(c).string = sym; return c; }
cell_t *set_ffun(cell_t *c, ffun_t *f) { type(c) = T_FFUN; as(c).ffun = f; return c; }
cell_t *set_vector(cell_t *c, vector_t *v) { type(c) = T_VECTOR; as(c).vector = v; return c; }
cell_t *set_table(cell_t *c, vector_t *v) { type(c) = T_TABLE; as(c).vector = v; return c; }
cell_t *set_port(cell_t *c, FILE *p) { type(c) = T_PORT; as(c).port = p; return c; }

cell_t *cons(SchemeY *s, cell_t *_car, cell_t *_cdr) { return set_cons(obj_alloc(s), _car, _cdr); }
cell_t *mk_int(SchemeY *s, long i) { return set_int(obj_alloc(s), i); }
cell_t *mk_real(SchemeY *s, float r) { return set_real(obj_alloc(s), r); }
cell_t *mk_char(SchemeY *s, int ch) { return set_char(obj_alloc(s), ch); }
cell_t *mk_string(SchemeY *s, char *str) { return set_string(obj_alloc(s), heap_strdup(s, str)); }
cell_t *mk_symbol(SchemeY *s, char *sym) { return set_symbol(obj_alloc(s), heap_strdup(s, sym)); }
cell_t *mk_ffun(SchemeY *s, ffun_t *f) { return set_ffun(obj_alloc(s), f); }
cell_t *mk_vector(SchemeY *s, size_t sz) { return set_vector(obj_alloc(s), mk_vector_t(s, sz)); }
cell_t *mk_table(SchemeY *s, size_t sz) { return set_table(obj_alloc(s), mk_vector_t(s, sz)); }
cell_t *mk_port(SchemeY *s, FILE *p) { return set_port(obj_alloc(s), p); }

vector_t *mk_vector_t(SchemeY *s, size_t sz) {
  vector_t *v = heap_malloc(s, sizeof(vector_t));
  v->_items = heap_calloc(s, sz, sizeof(cell_t));
  v->_len = 0;
  v->_size = sz;
  return v;
}
