#include "utils.h"

#include <stdio.h>
#include <stdlib.h>

// Raw malloc, calling exit on failure.
void *err_malloc(size_t size) {
  void *p = malloc(size);
  if (!p) {
    perror("malloc()");
    exit(1);
  }
  return p;
}

// Raw calloc, calling exit on failure.
void *err_calloc(size_t n, size_t size) {
  void *p = calloc(n, size);
  if (!p) {
    perror("calloc()");
    exit(1);
  }
  return p;
}

// Raw realloc, calling exit on failure.
void *err_realloc(void *ptr, size_t size) {
  void *p = realloc(ptr, size);
  if (!p) {
    perror("realloc()");
    exit(1);
  }
  return p;
}

/* djb2 hash: http://www.cse.yorku.ca/~oz/hash.html */
unsigned long hash(const char *s) {
  unsigned long h = 5381;
  while (*s) {
    /* h = h * 33 ^ c */
    h = ((h << 5) + h) ^ *s++;
  }
  return h;
}
