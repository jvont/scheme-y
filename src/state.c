#include "state.h"
#include "mem.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void syS_init(SchemeY *s) {
  s->heap = syM_new(HEAP_SIZE * sizeof(cell_t));
  s->global_env = syO_table(s, GLOBAL_ENV_SIZE);
  s->env = syO_cons(s, s->global_env, NULL);
  s->input_port = syO_port(s, stdin, "r");
  s->output_port = syO_port(s, stdin, "w");
}

void syS_shutdown(SchemeY *s) {
  syM_free(s->heap);
}

// djb2 hash: http://www.cse.yorku.ca/~oz/hash.html
static unsigned int hash(const char *s) {
  unsigned int h = 5381;
  while (*s) {
    // h = h * 33 ^ c
    h = ((h << 5) + h) ^ *s++;
  }
  return h;
}

// Lookup a given variable and return its entry, or NULL if not found.
// FUTURE: convert to hash-table-eq/get
cell_t *syS_lookup(SchemeY *s, cell_t *var) {
  cell_t *t = s->global_env;
  unsigned int size = getv(t).vector->size;
  unsigned int i = hash(getv(var).string) % size;
  cell_t *ev = getv(t).vector->items;
  for (; car(ev + i); i = (i + 1) % size) {
    if (car(ev + i) == var)
      return ev + i;
  }
  return NULL;
}

// Find/store a symbol, returning its cell.
cell_t *syS_intern(SchemeY *s, char *sym) {
  cell_t *t = s->global_env;
  unsigned int size = getv(t).vector->size;
  unsigned int i = hash(sym) % size;
  cell_t *ev = getv(t).vector->items;
  for (; car(ev + i); i = (i + 1) % size) {
    if (strcmp(sym, getv(car(ev + i)).string) == 0)
      return car(ev + i);
  }
  return (car(ev + i) = syO_symbol(s, sym));
}

cell_t *map(SchemeY *s, cell_t *args) {
  // check arity
  cell_t *fn = car(args);
  for (args = cdr(args); cdr(args); args = cdr(args)) {

  }
}

cell_t *apply(SchemeY *s, cell_t *args) {
  cell_t *fn = car(args);
  cell_t *body = cdr(args);
  // switch (fn->kind) {
  //   case TyFFun:
  //   case TyClosure:
  //   default:
  // }
}

cell_t *eval_list(SchemeY *s, cell_t *args) {
  cell_t *first = syS_eval(s, car(args));
  cell_t *rest = cdr(args) ? eval_list(s, cdr(args)) : NULL;
  return syO_cons(s, first, rest);
}

// Evaluate an expression.
cell_t *syS_eval(SchemeY *s, cell_t *expr) {
  if (!expr)  // empty
    return NULL;
  else if (iscons(expr)) {
    cell_t *p = syS_eval(s, car(expr));
    if (!p)
      return NULL;
    else if (gett(p) == FFUN) {  // foreign-func
      cell_t *args = eval_list(s, cdr(expr));
      return getv(p).ffun(s, args);
    }
    else return NULL;
  }
  else if (gett(expr) == SYMBOL)
    return cdr(syS_lookup(s, expr));
  else
    return expr;
}
