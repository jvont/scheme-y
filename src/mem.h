/*
** Memory manager (heap).
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

/* convert size in bytes to objects, rounded up */
#define objsize(n) ((n + sizeof(Object) - 1) / sizeof(Object))

// typedef struct Generation {
//   Object *roots;
//   Object *heap;
//   Object *next;
//   size_t size;
//   struct Generation *to;
// } Generation;

/* managed heap global variables */
extern Object *heap, *heap2;
extern Object *next;
extern size_t semi;

#define ROOTS_SIZE 8
extern Object ***roots;
extern size_t roots_len, roots_size;

void mem_init(int n_generations);
void mem_shutdown();

void mem_root(Object **root);

void *mem_malloc(size_t size);
void *mem_calloc(size_t n, size_t size);

char *mem_strdup(const char *s);
char *mem_strndup(const char *s, size_t n);

// void garbage_collect(int generation);
void garbage_collect(int level);

#endif