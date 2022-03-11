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
#include "state.h"

#include <stdlib.h>

// TODO: dynamic resize
// TODO: ensure allocated strings are cleaned up properly
// TODO: move heap directly to state

cell_t *obj_alloc(SchemeY *s);
void *obj_calloc(SchemeY *s, size_t n, size_t size);

void *heap_malloc(SchemeY *s, size_t n);
char *heap_strdup(SchemeY *s, char *src);
char *heap_strndup(SchemeY *s, char *src, size_t n);

void gc(SchemeY *s);

#endif