#ifndef _RUNTIME_H
#define _RUNTIME_H

#include "object.h"

// TODO: GC

void *mem_alloc(size_t size);
void *mem_calloc(size_t n, size_t size);
void *mem_realloc(void *ptr, size_t size);

Env *env_new();
void env_free(Env *e);

// Obj* intern(Obj **symbols, char *sym);

#endif