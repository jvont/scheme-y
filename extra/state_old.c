#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void sy_init(State *s) {
  /* global variables (internal structure, never referenced) */
  size_t vs = objsize(sizeof(Vector));
  s->globals = calloc(vs + GLOBAL_ENV_SIZE, sizeof(Cell));
  if (!s->globals) exit(1);  
  s->globals->len = 0;
  s->globals->size = GLOBAL_ENV_SIZE;

  /* default ports (collected, since they may be referenced) */
  s->inport = mk_port(stdin);
  s->outport = mk_port(stdout);

  /* token read buffer (resizeable) */
  s->token = malloc(BUFSIZ);
  if (!s->token) exit(1);
  s->tend = s->token + 10;
}

void sy_shutdown(State *s) {
  free(s->heap < s->heap2 ? s->heap : s->heap2);
  free(s->token);
  // if (s->globals)
  //   free(s->globals.items);
  // free(s->globals);
  // free(s->inport);
  // free(s->outport);
}

/* djb2 hash: http://www.cse.yorku.ca/~oz/hash.html */
static unsigned int hash(const char *s) {
  unsigned int h = 5381;
  while (*s) {
    /* h = h * 33 ^ c */
    h = ((h << 5) + h) ^ *s++;
  }
  return h;
}

/* Lookup a given variable by address and return its entry, or NULL if not found. */
static Cell *sy_lookup_entry(State *s, Cell *var) {
  Vector *v = s->globals;
  unsigned int size = v->size;
  unsigned int i = hash(as(var).string) % size;
  Cell *ev = v->items;
  for (; car(ev + i); i = (i + 1) % size) {
    if (car(ev + i) == var)
      return ev + i;
  }
  return NULL;
}

/* Lookup a given variable by address and return its value, or NULL if not found. */
// FUTURE: convert to hash-table-eq/get
Cell *sy_lookup(State *s, Cell *var) {
  Cell *e = sy_lookup_entry(s, var);
  return (!e) ? NULL : cdr(e);
}

Cell *sy_bind(State *s, Cell *var, Cell *val) {
  Cell *e = sy_lookup_entry(s, var);
  if (!e)  // ERROR
    return NULL;
  cdr(e) = val;
  return NULL;
}

/* Find/store a symbol, returning its associated entry. */
Cell *sy_intern_entry(State *s, char *sym) {
  Vector *v = s->globals;
  unsigned int size = v->size;
  unsigned int i = hash(sym) % size;
  Cell *ev = v->items;
  for (; car(ev + i); i = (i + 1) % size) {
    if (strcmp(sym, as(car(ev + i)).string) == 0)
      return ev + i;
  }
  car(ev + i) = mk_symbol(sym);
  return ev + i;
}

/* Find/store a symbol and return it. */
Cell *sy_intern(State *s, char *sym) {
  return car(sy_intern_entry(s, sym));
}

Cell *sy_intern_bind(State *s, char *sym, Cell *val) {
  Cell *e = sy_intern_entry(s, sym);
  cdr(e) = val;
  return e;
}

Cell *map(State *s, Cell *args) {
  // TODO: check arity
  Cell *fn = car(args);
  for (args = cdr(args); cdr(args); args = cdr(args)) {

  }
}

Cell *apply(State *s, Cell *args) {
  Cell *fn = car(args);
  Cell *body = cdr(args);
  // switch (fn->kind) {
  //   case TyFFun:
  //   case TyClosure:
  //   default:
  // }
}

Cell *eval_list(State *s, Cell *args) {
  Cell *first = sy_eval(s, car(args));
  Cell *rest = cdr(args) ? eval_list(s, cdr(args)) : NULL;
  return cons(first, rest);
}

/* Evaluate an expression. */
Cell *sy_eval(State *s, Cell *expr) {
  if (!expr) return NULL;  /* empty */
  else if (islist(expr)) {  /* list */


  // else return expr;

    Cell *p = sy_eval(s, car(expr));
    if (!p)
      return NULL;
    else if (isprocedure(p)) {  /* procedure */
      Cell *args = eval_list(s, cdr(expr));
      return as(p).ffun(s, args);
    }
    else return NULL;
  }
  else if (issymbol(expr))
    return sy_lookup(s, expr);
  else
    return expr;
}
