/*
** Managed heap.
**
** The heap uses a generational garbage collector to manage memory. New
** objects are (typically) allocated in the yougest generation (g0). Memory
** is allocated at the next available pointer (alloc) of the given generation.
** All special data structures (strings, vectors, etc.) are stored in
** contiguous memory, aligned to the size of a cell.
**
** On collection of a specific generation, objects from younger generations
** are copied to the next generation. The oldest generation (gn) is made up of
** two generations, which are used as swap spaces. Copying collection is
** performed using Cheney's algorithm.
*/
#ifndef HEAP_H
#define HEAP_H

#include "object.h"

#include <stddef.h>

// Number of generations (total generations - 1)
#define N_GENERATIONS 2

// Default generation size (resizeable)
#define GENERATION_SIZE 1024

// Convert size in bytes to size in objects, rounded up
#define objsize(n) ((n + sizeof(Object) - 1) / sizeof(Object))

typedef struct Generation {
  Object *blocks, *alloc;
  size_t size;
} Generation;

typedef struct Heap {
  Generation g0[N_GENERATIONS + 1];
  Generation *gn;  // current tenured swap space
  SyState *s;  // associated state (roots)
} Heap;

Heap *Heap_new(SyState *s);
void Heap_free(Heap *h);

// void *Heap_genmalloc(Heap *h, int gen, size_t size);
// void *Heap_gencalloc(Heap *h, int gen, size_t n, size_t size);
// void *Heap_genrealloc(Heap *h, int gen, void *ptr, size_t size);

void *Heap_malloc(Heap *h, size_t size);
void *Heap_calloc(Heap *h, size_t n, size_t size);
// void *Heap_realloc(Heap *h, void *ptr, size_t size);

char *Heap_strdup(Heap *h, const char *s);
char *Heap_strndup(Heap *h, const char *s, size_t n);

void Heap_collect(Heap *h, int gen);

#endif