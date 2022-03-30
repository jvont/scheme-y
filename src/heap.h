/*
** Managed heap.
**
** The heap uses a generational garbage collector to manage memory. New
** objects are usually allocated in the youngest generation (g0). Memory
** is allocated at the next available space (alloc) of the given generation.
** All special data structures (strings, vectors, etc.) are stored in
** contiguous memory, aligned to the size of a cell. Roots used for garbage
** collection are stored in an associated state (s).
**
** On collection of a specific generation, objects from younger generations
** are copied to the next generation, while older generations are ignored.
** The oldest generation (gn) is made up of two generations, which are used
** as swap spaces for collection.
*/
#ifndef HEAP_H
#define HEAP_H

#include "object.h"

#include <stddef.h>

#define DEFAULT_CHUNK_SIZE 1024
#define FULL_GC_AFTER 4

// Convert size in bytes to size in objects, rounded up
#define objsize(n) ((n + sizeof(Object) - 1) / sizeof(Object))

typedef struct Chunk Chunk;

typedef struct Heap {
  Chunk *g0, *g1, *swap;  // generations
  SyState *s;
  unsigned int after;  // full sweep counter
} Heap;

Heap *Heap_new(SyState *s);
void Heap_free(Heap *h);

void *Heap_malloc(Heap *h, size_t size);
void *Heap_calloc(Heap *h, size_t n, size_t size);
// void *Heap_realloc(Heap *h, void *ptr, size_t size);

// void *Heap_malloc2(Heap *h, size_t size);
// void *Heap_calloc2(Heap *h, size_t n, size_t size);
// void *Heap_realloc2(Heap *h, void *ptr, size_t size);

char *Heap_strdup(Heap *h, const char *s);
char *Heap_strndup(Heap *h, const char *s, size_t n);

void Heap_collect(Heap *h);

#endif