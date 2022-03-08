#include "object.h"
#include "state.h"
#include "mem.h"

cell_t *cons(SchemeY *s, cell_t *_car, cell_t *_cdr) {
  cell_t *c = syM_alloc(s); set_cons(c, _car, _cdr); return c;
}

cell_t *mk_int(SchemeY *s, int _int) {
  cell_t *c = syM_alloc(s); set_int(c, _int); return c;
}

cell_t *mk_real(SchemeY *s, float _real) {
  cell_t *c = syM_alloc(s); set_real(c, _real); return c;
}

cell_t *mk_char(SchemeY *s, int _char) {
  cell_t *c = syM_alloc(s); set_char(c, _char); return c;
}

cell_t *mk_string(SchemeY *s, char *_string) {
  cell_t *c = syM_alloc(s); set_string(c, syM_strdup(s, _string)); return c;
}

cell_t *mk_symbol(SchemeY *s, char *_symbol) {
  cell_t *c = syM_alloc(s); set_symbol(c, syM_strdup(s, _symbol)); return c;
}

cell_t *mk_ffun(SchemeY *s, ffun_t *_ffun) {
  cell_t *c = syM_alloc(s); set_ffun(c, _ffun); return c;
}

static vector_t *mk_vector_t(SchemeY *s, size_t _size) {
  vector_t *v = syM_malloc(s, sizeof(vector_t));
  v->items = syM_calloc(s, _size, sizeof(cell_t));
  v->len = 0;
  v->size = _size;
  return v;
}

cell_t *mk_vector(SchemeY *s, size_t _size) {
  cell_t *c = syM_alloc(s); set_vector(c, mk_vector_t(s, _size)); return c;
}

cell_t *mk_table(SchemeY *s, size_t _size) {
  cell_t *c = syM_alloc(s); set_table(c, mk_vector_t(s, _size)); return c;
}

cell_t *mk_port(SchemeY *s, FILE *_port) {
  cell_t *c = syM_alloc(s); set_port(c, _port); return c; 
}
