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
#ifndef HEAP_H
#define HEAP_H

#include "object.h"

#include <stddef.h>

#define HEAP_SIZE 128

// convert size in bytes to size in objects, rounded up
#define objsize(n) ((n + sizeof(Object) - 1) / sizeof(Object))

typedef struct Heap Heap;

typedef struct Chunk {
  Object *blocks;
  size_t size;
  struct Chunk *prev;
} Chunk;

struct Heap {
  Object *from, *to;  // semi-spaces
  Object *next;  // next free block
  size_t size;  // semi-space size

  // Chunk *from;  // linked list of chunks

  State *s;  // asoociated state
};

// procedure:
// from- and to-spaces are chunks
// allocate chunks when needed
// on collection, resize to-space if needed

Heap *Heap_new();
void Heap_free(Heap *h);

void *Heap_malloc(Heap *h, size_t size);
void *Heap_calloc(Heap *h, size_t n, size_t size);

char *Heap_strdup(Heap *h, const char *s);
char *Heap_strndup(Heap *h, const char *s, size_t n);

void Heap_collect(Heap *h);

#endif