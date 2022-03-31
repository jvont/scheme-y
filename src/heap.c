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
  void *blocks, *alloc;
  size_t size;
  struct Chunk *prev;  // prev chunk in list
} Chunk;

static Chunk *Chunk_new(size_t size) {
  Chunk *c = err_malloc(sizeof(Chunk));
  c->blocks = err_malloc(size);
  c->alloc = c->blocks;
  c->size = size;
  c->prev = NULL;
  return c;
}

static void Chunk_free(Chunk *c) {
  if (!c) return;
  Chunk_free(c->prev);
  free(c);
}

static size_t Chunk_count(Chunk *c) {
  if (!c)
    return 0;
  else
    return ((char *)c->alloc - (char *)c->blocks) + Chunk_count(c->prev);
}

static size_t Chunk_size(Chunk *c) {
  if (!c)
    return 0;
  else
    return c->size + Chunk_size(c->prev);
}

static int Chunk_contains(Chunk *c, void *x) {
  if (!c)
    return 0;
  else if (x >= c->blocks && x < c->alloc)
    return 1;
  return Chunk_contains(c->prev, x);
}

// ---------------------------------------------------------------------------
// Generation utilities
// ---------------------------------------------------------------------------

typedef struct Generation {
  Chunk *obj;  // scanned objects
  Chunk *data;  // general-purpose memory (malloc)
} Generation;

static Generation *Generation_new(size_t objects_size, size_t data_size) {
  Generation *g = err_malloc(sizeof(Generation));
  g->obj = Chunk_new(objects_size);
  g->data = Chunk_new(data_size);
  return g;
}

static void Generation_free(Generation *g) {
  Chunk_free(g->obj);
  Chunk_free(g->data);
  free(g);
}

static size_t Generation_count(Generation *g) {
  return Chunk_count(g->obj) + Chunk_count(g->data);
}

static size_t Generation_size(Generation *g) {
  return Chunk_size(g->obj) + Chunk_size(g->data);
}

// ---------------------------------------------------------------------------
// Heap definitions
// ---------------------------------------------------------------------------


struct Heap {
  Generation *g0, *g1, *swap;
  SyState *s;
  unsigned int after;  // full sweep counter
};

// Create a new heap with associated state.
Heap *Heap_new(SyState *s) {
  Heap *h = err_malloc(sizeof(Heap));
  h->g0 = Generation_new(DEFAULT_HEAP_SIZE, DEFAULT_HEAP_SIZE);
  h->g1 = Generation_new(DEFAULT_HEAP_SIZE, DEFAULT_HEAP_SIZE);
  h->swap = Generation_new(DEFAULT_HEAP_SIZE, DEFAULT_HEAP_SIZE);
  h->s = s;
  h->after = FULL_GC_AFTER;
  return h;
}

void Heap_free(Heap *h) {
  Generation_free(h->g0);
  Generation_free(h->g1);
  Generation_free(h->swap);
  free(h);
}

// Allocate a new scanned object.
Object *Heap_object(Heap *h) {
  Chunk *c0 = h->g0->obj;
  if ((Object *)c0->alloc >= (Object *)c0->blocks + c0->size) {
    Chunk *c = Chunk_new(DEFAULT_HEAP_SIZE);
    c->prev = c0;
    h->g0->obj = c;
  }
  Object *p = h->g0->obj->alloc;
  h->g0->obj->alloc = p + 1;
  return p;
}

// Allocate size bytes of general purpose memory.
void *Heap_malloc(Heap *h, size_t size) {
  if (size == 0) return NULL;
  Chunk *c0 = h->g0->data;
  if ((char *)c0->alloc + size >= (char *)c0->blocks + c0->size) {
    Chunk *c = Chunk_new(MAX(size, DEFAULT_HEAP_SIZE));
    c->prev = c0;
    h->g0->data = c;
  }
  void *p = h->g0->data->alloc;
  h->g0->data->alloc = (char *)p + size;
  return p;
}

// Allocate an array of n objects of size bytes, setting their bytes to zero.
void *Heap_calloc(Heap *h, size_t n, size_t size) {
  void *p = Heap_malloc(h, n * size);
  return p ? memset(p, 0, n * size) : NULL;
}

char *Heap_strdup(Heap *h, const char *s) {
  return Heap_strndup(h, s, strlen(s));
}

char *Heap_strndup(Heap *h, const char *s, size_t n) {
  char *d = Heap_malloc(h, n + 1);
  return strcpy(d, s);
}

// Get the allocated object count.
size_t Heap_count(Heap *h) {
  return Generation_count(h->g0) + Generation_count(h->g1);
}

// Get the total heap size.
size_t Heap_size(Heap *h) {
  return (Generation_size(h->g0) + 
          Generation_size(h->g1) +
          Generation_size(h->swap));
}

// ---------------------------------------------------------------------------
// Garbage collection
// ---------------------------------------------------------------------------

static void copy_obj(Heap *h, Object **p) {
  Object *fwd, *x = *p;
  if (!x)
    return;
  else if (islist(x)) {
    fwd = Heap_object(h);
    *fwd = *x;
    copy_obj(h, &car(fwd));
    copy_obj(h, &cdr(fwd));
  }
  else switch (type(x)) {
    case T_STRING: {
      fwd = Heap_object(h);
      *fwd = *x;
      as(fwd).string = Heap_strdup(h, as(x).string);
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
      fwd = Heap_object(h);
      *fwd = *x;
      break;
    }
  }
  type(x) = T_FWD;
  as(x).fwd = fwd;
  *p = fwd;
}

static void compact_chunk(Chunk *c, size_t size) {
  Chunk_free(c->prev);
  c->prev = NULL;
  if (c->size < size) {
    c->blocks = err_realloc(c->blocks, size);
    c->alloc = c->blocks;
    c->size = size;
  }
}

// Compact swap space of heap based on current total heap size.
static void compact_swap(Heap *h) {
  size_t obj_count = Chunk_count(h->g0->obj) + Chunk_count(h->g1->obj);
  compact_chunk(h->swap->obj, obj_count);
  size_t data_count = Chunk_count(h->g0->data) + Chunk_count(h->g1->data);
  compact_chunk(h->swap->data, data_count);
}

void Heap_collect(Heap *h) {
  SyState *s = h->s;

  // swap young generation
  Generation *swap = h->g0;
  h->g0 = h->swap;
  h->swap = swap;

  // set scan/alloc pointers
  Chunk *c0 = h->g0->obj;
  Object *scan = c0->alloc = c0->blocks;

  // copy roots
  // TODO: figure out what roots look like
  // TODO: scan pointer + chunks issue
  for (size_t i = 0; i < s->top; i++) {
    copy_obj(h, s->stack + i);
  }

  // for (; scan != c0->alloc; scan++) {
  //   copy_obj(h, &scan);
  // }

  // compact swap generation into single chunk
  compact_swap(h);
}
