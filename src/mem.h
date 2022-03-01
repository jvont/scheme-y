/*
** Memory manager (heap).
** Memory is allocated at the next available pointer on the heap.
** A copying collector is used, with collection performed using
** Cheney's algorithm.
*/
#ifndef _SY_MEM_H
#define _SY_MEM_H

#include "object.h"

#include <stdlib.h>

// TODO: dynamic resize
// TODO: ensure allocated strings are cleaned up properly

typedef struct Heap {
  void *from, *to;  // semi-spaces
  void *next;  // free memory head
  size_t size;  // half-size, in bytes
} Heap;

Heap *syM_new(size_t size);
void syM_free(Heap *h);

void *syM_alloc(SchemeY *s, size_t n);
void *syM_calloc(SchemeY *s, size_t n, size_t size);

char *syM_strdup(SchemeY *s, char *src);
char *syM_strndup(SchemeY *s, char *src, size_t n);

void syM_gc(SchemeY *s);

#endif