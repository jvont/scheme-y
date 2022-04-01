/*
** Managed heap.
**
** The heap uses a generational garbage collector to manage memory. New
** objects are usually allocated in the youngest generation (g0). Memory
** is allocated at the next available space (alloc) of the given generation.
** All special data structures (strings, vectors, etc.) are stored in a
** separate chunk from objects, which are scanned during garbage collection
** using Cheney's algorithm.
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

#define DEFAULT_HEAP_SIZE 1024
#define FULL_GC_AFTER 4

// Convert size in bytes to size in objects, rounded up
#define objsize(n) ((n + sizeof(Object) - 1) / sizeof(Object))

typedef struct Heap Heap;

Heap   *Heap_new(SyState *s);
void    Heap_free(Heap *h);

size_t  Heap_count(Heap *h);
size_t  Heap_size(Heap *h);

Object *Heap_object(Heap *h);
void   *Heap_malloc(Heap *h, size_t size);
void   *Heap_calloc(Heap *h, size_t n, size_t size);
char   *Heap_strdup(Heap *h, const char *s);
char   *Heap_strndup(Heap *h, const char *s, size_t n);

void    Heap_collect(Heap *h);

#endif