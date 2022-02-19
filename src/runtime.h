#ifndef _RUNTIME_H
#define _RUNTIME_H

#include "object.h"
#include "scanner.h"

// TODO: GC

void *mem_alloc(size_t size);
void *mem_calloc(size_t n, size_t size);
void *mem_realloc(void *ptr, size_t size);

typedef struct Env {
  Object *symbols;  // bound symbols
  Object *heap;  // allocated objects
  Scanner *s;  // token scanner
} Env;

Env *env_new();
void env_free(Env *e);

// Object* intern(Object **symbols, char *sym);

#endif