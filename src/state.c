#include "state.h"
#include "mem.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

// TODO: allocate global variables outside of managed heap (they are never collected)
void sy_init(SchemeY *s) {
  /* allocate heap first */
  s->heap = malloc(2 * HEAP_SIZE * sizeof(cell_t));
  if (!s->heap) exit(1);
  s->heap2 = s->heap + HEAP_SIZE;
  s->semi = HEAP_SIZE;
  /* global variables */
  s->globals = mk_vector_t(s, GLOBAL_ENV_SIZE);
  s->inport = mk_port(s, stdin);
  s->outport = mk_port(s, stdout);
  /* token buffer */
  s->token = malloc(BUFSIZ);
  if (!s->token) exit(1);
  s->tend = s->token + 10;
}

void sy_shutdown(SchemeY *s) {
  free(s->heap < s->heap2 ? s->heap : s->heap2);
  free(s->token);
  // if (s->globals)
  //   free(s->globals._items);
  // free(s->globals);
  // free(s->inport);
  // free(s->outport);
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

// Lookup a given variable by address and return its entry, or NULL if not found.
static cell_t *sy_lookup_entry(SchemeY *s, cell_t *var) {
  vector_t *v = s->globals;
  unsigned int size = v->_size;
  unsigned int i = hash(as(var).string) % size;
  cell_t *ev = v->_items;
  for (; car(ev + i); i = (i + 1) % size) {
    if (car(ev + i) == var)
      return ev + i;
  }
  return NULL;
}

// Lookup a given variable by address and return its value, or NULL if not found.
// FUTURE: convert to hash-table-eq/get
cell_t *sy_lookup(SchemeY *s, cell_t *var) {
  cell_t *e = sy_lookup_entry(s, var);
  return (!e) ? NULL : cdr(e);
}

cell_t *sy_bind(SchemeY *s, cell_t *var, cell_t *val) {
  cell_t *e = sy_lookup_entry(s, var);
  if (!e)  // ERROR
    return NULL;
  cdr(e) = val;
  return NULL;
}

// Find/store a symbol, returning its associated entry.
cell_t *sy_intern_entry(SchemeY *s, char *sym) {
  vector_t *v = s->globals;
  unsigned int size = v->_size;
  unsigned int i = hash(sym) % size;
  cell_t *ev = v->_items;
  for (; car(ev + i); i = (i + 1) % size) {
    if (strcmp(sym, as(car(ev + i)).string) == 0)
      return ev + i;
  }
  car(ev + i) = mk_symbol(s, sym);
  return ev + i;
}

// Find/store a symbol and return it.
cell_t *sy_intern(SchemeY *s, char *sym) {
  return car(sy_intern_entry(s, sym));
}

cell_t *sy_intern_bind(SchemeY *s, char *sym, cell_t *val) {
  cell_t *e = sy_intern_entry(s, sym);
  cdr(e) = val;
  return NULL;
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
  cell_t *first = sy_eval(s, car(args));
  cell_t *rest = cdr(args) ? eval_list(s, cdr(args)) : NULL;
  return cons(s, first, rest);
}

// Evaluate an expression.
cell_t *sy_eval(SchemeY *s, cell_t *expr) {
  if (!expr) return NULL;  // empty
  else if (iscons(expr)) {  // list


  // else return expr;

    cell_t *p = sy_eval(s, car(expr));
    if (!p)
      return NULL;
    else if (isffun(p)) {  // foreign-func
      cell_t *args = eval_list(s, cdr(expr));
      return as(p).ffun(s, args);
    }
    else return NULL;
  }
  else if (issymbol(expr))
    return sy_lookup(s, expr);
  else
    return expr;
}
