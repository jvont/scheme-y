/*
** Memory manager (heap).
** Memory is allocated at the next available pointer on the heap. All special
** data structures (strings, vectors, etc.) are stored in contiguous memory,
** aligned to the size of a cell. Copying collection is performed, using
** Cheney's algorithm.
*/
#ifndef _SY_MEM_H
#define _SY_MEM_H

#include "object.h"

#include <stdlib.h>

// TODO: dynamic resize
// TODO: ensure allocated strings are cleaned up properly

struct heap {
  cell_t *from, *to;  // semi-spaces
  cell_t *next;  // free memory head
  size_t size;  // half-size, in bytes
};

heap_t *syM_new(size_t size);
void syM_free(heap_t *h);

cell_t *syM_alloc(SchemeY *s);
void *syM_malloc(SchemeY *s, size_t n);
void *syM_calloc(SchemeY *s, size_t n, size_t size);

char *syM_strdup(SchemeY *s, char *src);
char *syM_strndup(SchemeY *s, char *src, size_t n);

void syM_gc(SchemeY *s);

#endif