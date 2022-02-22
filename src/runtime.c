#include "runtime.h"

#include <stdlib.h>
#include <string.h>

// malloc with exit(1) on failure.
void *mem_alloc(size_t size) {
  void *ptr = malloc(size);
  if (ptr == NULL) {
    perror("malloc failure");
    exit(1);
  }
  return ptr;
}

// calloc with exit(1) on failure.
void *mem_calloc(size_t n, size_t size) {
  void *ptr = calloc(n, size);
  if (ptr == NULL) {
    perror("calloc failure");
    exit(1);
  }
  return ptr;
}

// remalloc with exit(1) on failure.
void *mem_realloc(void *ptr, size_t size) {
  void *nptr = realloc(ptr, size);
  if (nptr == NULL) {
    perror("realloc failure");
    exit(1);
  }
  return ptr;
}

Env *env_new() {
  Env *e = (Env *)mem_alloc(sizeof(Env));
  e->bindings = NULL;
  return e;
}

void env_free(Env *e) {
  // free(e->heap);
  free(e);
}
