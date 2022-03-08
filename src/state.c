#include "state.h"
#include "mem.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void syS_init(SchemeY *s) {
  s->heap = syM_new(HEAP_SIZE * sizeof(cell_t));
  s->globals = mk_table(s, GLOBAL_ENV_SIZE);
  s->env = cons(s, s->globals, NULL);
  s->inport = mk_port(s, stdin);
  s->outport = mk_port(s, stdout);

  s->tbase = s->tptr = malloc(10);
  if (!s->tbase) exit(1);
  s->tend = s->tbase + 10;

  s->lookahead = s->prompt ? '\n' : EOF;
  s->lineno = 1;
  s->depth = 0;
  s->err = E_OK;
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

// Lookup a given symbol by address and return its entry, or NULL if not found.
// FUTURE: convert to hash-table-eq/get
cell_t *syS_lookup(SchemeY *s, cell_t *var) {
  vector_t *v = as(s->globals)._vector;
  unsigned int size = v->_size;
  unsigned int i = hash(as(var)._string) % size;
  cell_t *ev = v->_items;
  for (; car(ev + i); i = (i + 1) % size) {
    if (car(ev + i) == var)
      return ev + i;
  }
  return NULL;
}

// Find/store a symbol, returning its associated item.
cell_t *syS_intern_item(SchemeY *s, char *sym) {
  vector_t *v = as(s->globals)._vector;
  unsigned int size = v->_size;
  unsigned int i = hash(sym) % size;
  cell_t *ev = v->_items;
  for (; car(ev + i); i = (i + 1) % size) {
    if (strcmp(sym, as(car(ev + i))._string) == 0)
      return ev + i;
  }
  car(ev + i) = mk_symbol(s, sym);
  return ev + i;
}

// Find/store a symbol and return it.
cell_t *syS_intern(SchemeY *s, char *sym) {
  return car(syS_intern_item(s, sym));
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
  return cons(s, first, rest);
}

// Evaluate an expression.
cell_t *syS_eval(SchemeY *s, cell_t *expr) {
  if (!expr) return NULL;  // empty
  else if (iscons(expr)) {  // list


  // else return expr;

    cell_t *p = syS_eval(s, car(expr));
    if (!p)
      return NULL;
    else if (isffun(p)) {  // foreign-func
      cell_t *args = eval_list(s, cdr(expr));
      return as(p)._ffun(s, args);
    }
    else return NULL;
  }
  else if (issymbol(expr))
    return cdr(syS_lookup(s, expr));
  else
    return expr;
}
