/*
** Managed heap.
*/
#ifndef HEAP_H
#define HEAP_H

#include "object.h"

#include <stddef.h>

#define HEAP_MIN 1024
#define FULL_GC_AFTER 4

// Convert size in bytes to size in objects, rounded up
#define objsize(n) ((n + sizeof(Object) - 1) / sizeof(Object))

typedef struct Heap Heap;

Heap   *Heap_new(SyState *s);
void    Heap_free(Heap *h);

size_t  Heap_count(Heap *h);
size_t  Heap_size(Heap *h);

int Heap_location(Heap *h, Object *x);

Object *Heap_object(Heap *h);
void   *Heap_malloc(Heap *h, size_t size);
void   *Heap_calloc(Heap *h, size_t n, size_t size);
char   *Heap_strdup(Heap *h, const char *s);
char   *Heap_strndup(Heap *h, const char *s, size_t n);

void    Heap_collect(Heap *h);

#endif