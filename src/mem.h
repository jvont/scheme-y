/*
** Memory manager (heap).
** Memory is allocated at the next available pointer on the heap. All special
** data structures (strings, vectors, etc.) are stored in contiguous memory,
** aligned to the size of a cell. Copying collection is performed, using
** Cheney's algorithm.
*/
#ifndef _MEM_H
#define _MEM_H

#include "object.h"
#include "state.h"

#include <stdlib.h>

#define cellsize(s) ((s + sizeof(cell) - 1) / sizeof(cell))

// #define BLOCK_SIZE 128

// typedef struct generation {
//   cell *from, *to;
//   cell *next;
//   size_t semi;  // semi-space size
// } generation;

// extern generation *generations;
// extern generation *g0;

cell *obj_alloc(SchemeY *s);

void *heap_malloc(SchemeY *s, size_t size);
void *heap_calloc(SchemeY *s, size_t n, size_t size);
// void *heap_realloc(SchemeY *s, void *ptr, size_t size);

// void gc(SchemeY *s, int generation);
void gc(SchemeY *s);

#endif