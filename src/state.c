#include "state.h"
#include "mem.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void syS_init(SchemeY *s) {
  s->heap = syM_new(HEAP_SIZE * sizeof(cell));
  s->global_env = syO_table(s, GLOBAL_ENV_SIZE);
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
cell *syS_lookup(SchemeY *s, cell *var) {
  cell *t = s->global_env;
  unsigned int size = t->as.vector.size;
  unsigned int i = hash(var->as.string) % size;
  cell *ev = t->as.vector.items;
  for (; car(ev + i); i = (i + 1) % size) {
    if (car(ev + i) == var)
      return ev + i;
  }
  return NULL;
}

// Find/store a symbol, returning its cell.
cell *syS_intern(SchemeY *s, char *sym) {
  cell *t = s->global_env;
  unsigned int size = t->as.vector.size;
  unsigned int i = hash(sym) % size;
  cell *ev = t->as.vector.items;
  for (; car(ev + i); i = (i + 1) % size) {
    if (strcmp(sym, car(ev + i)->as.string) == 0)
      return car(ev + i);
  }
  return (car(ev + i) = syO_symbol(s, sym));
}

// cell *apply(SchemeY *s, cell *args) {
//   cell *fun = car(args);
//   cell *body = cdr(args);
// }

// Evaluate an expression.
cell *syS_eval(SchemeY *s, cell *expr) {
  if (!expr)  // empty
    return NULL;
  else if (expr->kind == TySymbol)  // lookup
    return cdr(syS_lookup(s, expr));
  else if (expr->kind == TyPair) {  // application
    cell *p = syS_eval(s, car(expr));
    if (!p) return NULL;
    cell *args = eval_list(cdr(expr));

    if (p->kind == TyFFun)
      return p->as.ffun(s, args);
    else
      return NULL;
  }
  else return expr;  // simple
}
