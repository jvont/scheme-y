/*
** Memory manager (heap).
**
** Memory is allocated at the next available pointer on the heap. All special
** data structures (strings, vectors, etc.) are stored in contiguous memory,
** aligned to the size of a cell. Copying collection is performed, using
** Cheney's algorithm.
**
** 
*/
#ifndef _MEM_H
#define _MEM_H

#include "object.h"

#include <stdlib.h>

#define HEAP_SIZE 32
#define ROOTS_SIZE 8
// #define GENERATION_SIZE 32

/* Converts size in bytes to size in objects, rounded up. */
#define objsize(n) ((n + sizeof(Object) - 1) / sizeof(Object))

typedef struct Generation {
  /* Memory blocks. */
  Object *blocks, *alloc;
  size_t blocks_size;
  /* Roots for this generation. */
  Object ***roots;
  size_t roots_len, roots_size;
  /* Next younger generation. */
  struct Generation *to;
} Generation;

typedef struct Heap {
  /* Generations array (oldest to youngest). */
  Generation *generations;
  Generation *g0;
  size_t n_generations;
} Heap;

extern Object *heap, *heap_next;

void mem_init(int n);
void mem_shutdown();

void mem_push(Object **root);
void mem_pop();

void *mem_malloc(size_t size);
void *mem_calloc(size_t n, size_t size);

char *mem_strdup(const char *s);
char *mem_strndup(const char *s, size_t n);

void garbage_collect(int n);

#endif