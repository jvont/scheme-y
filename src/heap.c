#include "heap.h"
#include "runtime.h"
#include "utils.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

// Convert size in bytes to size in objects, rounded up
#define objsize(n) ((n + sizeof(Object) - 1) / sizeof(Object))

// ---------------------------------------------------------------------------
// Chunk utilities
// ---------------------------------------------------------------------------

typedef struct Chunk {
  Object *blocks, *alloc;
  size_t size;
  struct Chunk *next;  // next chunk in list
} Chunk;

typedef struct Generation {
  Chunk *main;
} Generation;

static Chunk *Chunk_new(size_t size) {
  Chunk *c = err_malloc(sizeof(Chunk));
  c->blocks = err_malloc(size * sizeof(Object));
  c->alloc = c->blocks;
  c->size = size;
  c->next = NULL;
  return c;
}

static void Chunk_free(Chunk *c) {
  if (!c) return;
  Chunk_free(c->next);
  free(c);
}

static size_t Chunk_count(Chunk *c) {
  if (!c)
    return 0;
  else
    return (c->alloc - c->blocks) + Chunk_count(c->next);
}

static size_t Chunk_size(Chunk *c) {
  if (!c)
    return 0;
  else
    return c->size + Chunk_size(c->next);
}

static int Chunk_contains(Chunk *c, Object *x) {
  if (!c)
    return 0;
  else if (x >= c->blocks && x < c->alloc)
    return 1;
  return Chunk_contains(c->next, x);
}

// ---------------------------------------------------------------------------
// Heap definitions
// ---------------------------------------------------------------------------

struct Heap {
  Chunk *g0, *g1, *swap;  // generations
  SyState *s;
  unsigned int after;  // full sweep counter
};

// Create a new heap with associated state.
Heap *Heap_new(SyState *s) {
  Heap *h = err_malloc(sizeof(Heap));
  h->g0 = Chunk_new(DEFAULT_HEAP_SIZE);
  h->g1 = Chunk_new(DEFAULT_HEAP_SIZE);
  h->swap = Chunk_new(DEFAULT_HEAP_SIZE);
  h->s = s;
  h->after = FULL_GC_AFTER;
  return h;
}

void Heap_free(Heap *h) {
  Chunk_free(h->g0);
  Chunk_free(h->g1);
  Chunk_free(h->swap);
  free(h);
}

// Create a new object on the scanned heap.
Object *Heap_object(Heap *h) {

}

// Allocate size bytes of cell-aligned memory.
void *Heap_malloc(Heap *h, size_t size) {
  if (size == 0) return NULL;
  size_t n = objsize(size);
  Chunk *g0 = h->g0;
  if (g0->alloc + n >= g0->blocks + g0->size) {
    Chunk *c = Chunk_new(MAX(n, DEFAULT_HEAP_SIZE));
    c->next = g0;
    h->g0 = c;
  }
  Object *p = g0->alloc;
  g0->alloc += n;
  return p;
}

// Allocate an array of n objects of size bytes, setting their bytes to zero.
void *Heap_calloc(Heap *h, size_t n, size_t size) {
  void *p = Heap_malloc(h, n * size);
  return p ? memset(p, 0, n * size) : NULL;
}

// Get the allocated object count.
size_t Heap_count(Heap *h) {
  return Chunk_count(h->g0) + Chunk_count(h->g1);
}

// ---------------------------------------------------------------------------
// Garbage collection
// ---------------------------------------------------------------------------

static void copy_obj(Heap *h, Object **p) {
  Object *fwd, *x = *p;
  if (!x)
    return;
  else if (islist(x)) {
    fwd = Heap_malloc(h, sizeof(Object));
    *fwd = *((Object *)untag(x));
    copy_obj(h, &car(fwd));
    copy_obj(h, &cdr(fwd));
    fwd = (Object *)tag(fwd);
  }
  else switch (type(x)) {
    case T_STRING: {
      fwd = Heap_malloc(h, strlen(as(x).string) + sizeof(Object));
      *fwd = *x;
      as(fwd).string = strcpy((char *)(fwd + 1), as(x).string);
      break;
    }
    case T_VECTOR:
    case T_TABLE: {
      // TODO: copy an object's references (strings, vectors, etc.) to separate space,
      // allowing us to scan the main chunk with scan++

      

      // size_t size = as(x).vector->size;
      // fwd = Heap_malloc(h, sizeof(Vector) + size * sizeof(Object));
      // *fwd = *x;
      // Object *items = as(fwd).vector->items;
      // for (size_t i = 0; i < size; i++) {
      //   copy_obj(h, items + i);
      // }
      break;
    }
    case T_FWD: {
      *p = as(x).fwd;
      return;  // skip
    }
    default: {
      fwd = Heap_malloc(h, sizeof(Object));
      *fwd = *x;
      break;
    }
  }
  type(x) = T_FWD;
  as(x).fwd = fwd;
  *p = fwd;
}

static void compact_swap(Heap *h) {
  size_t total = Chunk_size(h->g0) + Chunk_size(h->g1);
  Chunk *c = h->swap;
  Chunk_free(c->next);
  c->blocks = err_realloc(c->blocks, total);
  c->alloc = c->blocks;
  c->next = NULL;
}

void Heap_collect(Heap *h) {
  SyState *s = h->s;

  // swap young generation
  Chunk *swap = h->g0;
  h->g0 = h->swap;
  h->swap = swap;

  Object *scan = h->g0->alloc = h->g0->blocks;

  // copy roots
  // TODO: figure out what roots look like
  // TODO: scan pointer + chunks issue
  for (size_t i = 0; i < s->top; i++) {
    copy_obj(h, s->stack + i);
  }

  // compact swap generation into single chunk
  compact_swap(h);
}
