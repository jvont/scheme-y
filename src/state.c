#include "state.h"
#include "mem.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void sy_init(SchemeY *s) {
  /* global variables (internal structure, never referenced) */
  s->globals = malloc(sizeof(vector_t));
  if (!s->globals) exit(1);
  s->globals->items = calloc(GLOBAL_ENV_SIZE, sizeof(cell_t));
  if (!s->globals->items) exit(1);
  s->globals->len = 0;
  s->globals->size = GLOBAL_ENV_SIZE;

  /* managed heap */
  s->heap = malloc(2 * HEAP_SIZE * sizeof(cell_t));
  if (!s->heap) exit(1);
  s->heap2 = s->heap + HEAP_SIZE;
  s->next = s->heap;
  s->semi = HEAP_SIZE;
  s->pin = NULL;

  /* default ports (collected, since they may be referenced) */
  s->inport = mk_port(s, stdin);
  s->outport = mk_port(s, stdout);

  /* token read buffer (resizeable) */
  s->token = malloc(BUFSIZ);
  if (!s->token) exit(1);
  s->tend = s->token + 10;
}

void sy_shutdown(SchemeY *s) {
  free(s->heap < s->heap2 ? s->heap : s->heap2);
  free(s->token);
  // if (s->globals)
  //   free(s->globals.items);
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
  unsigned int size = v->size;
  unsigned int i = hash(as(var).string) % size;
  cell_t *ev = v->items;
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
  unsigned int size = v->size;
  unsigned int i = hash(sym) % size;
  cell_t *ev = v->items;
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
  return e;
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
  else if (islist(expr)) {  // list


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
