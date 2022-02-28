#include "state.h"
#include "mem.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

void syS_init(SchemeY *s) {
  s->heap = syM_new(HEAP_SIZE);
  s->global_env = syO_table(s, GLOBAL_ENV_SIZE);
  s->symbols = syO_vector(s, SYMBOLS_SIZE);
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

// Lookup a given variable name, NULL if not found.
cell *syS_lookup(SchemeY *s, cell *var) {
  cell *t = s->global_env;
  unsigned int size = t->as.vector.size;
  unsigned int i = hash(var->as.string) % size;
  cell *ev = t->as.vector.items;
  for (; car(ev + i); i = (i + 1) % size) {
    if (car(ev + i) == var)
      return cdr(ev + i);
  }
  return NULL;
}

// Find/store a symbol, returning its cell location.
cell *syS_intern(SchemeY *s, char *sym) {
  cell *t = s->symbols;
  unsigned int size = t->as.vector.size;
  unsigned int i = hash(sym) % size;
  cell *ev = t->as.vector.items;
  for (; ev[i].kind; i = (i + 1) % size) {
    if (strcmp(sym, ev[i].as.string) == 0)
      return ev + i;
  }
  cell *e = ev + i;
  e->kind = TySymbol;
  e->as.string = syM_strdup(s, sym);
  return e;
}
