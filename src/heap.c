#include "heap.h"
#include "runtime.h"
#include "utils.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

// Chunks are a singly-linked list of heap memory. Each time memory is
// requested, the next available space in the list head is used. If there
// is not enough space for the memory requested, a new chunk is pushed onto
// the head of the list.
// On collection, the list of chunks is amalgamated into a single chunk with
// size based on current heap size.
typedef struct Chunk {
  void *blocks, *alloc;
  void *mark;  // previous collection end point
  size_t size;
  struct Chunk *prev;  // prev chunk in list
} Chunk;

static Chunk *Chunk_new(size_t size) {
  Chunk *c = err_malloc(sizeof(Chunk));
  c->blocks = err_malloc(size);
  c->alloc = c->blocks;
  c->mark = NULL;
  c->size = size;
  c->prev = NULL;
  return c;
}

static void Chunk_free(Chunk *c) {
  if (!c) return;
  Chunk_free(c->prev);
  free(c);
}

// Allocate size bytes of memory, pushing a new chunk onto the front of the
// list if needed.
static void *Chunk_alloc(Chunk **p, size_t size) {
  Chunk *c = *p;
  if (size == 0)
    return NULL;

  if ((char *)c->alloc + size >= (char *)c->blocks + c->size) {
    Chunk *cnew = Chunk_new(HEAP_MIN);
    cnew->prev = c;
    *p = cnew;
  }
  void *x = c->alloc;
  c->alloc = (char *)c->alloc + size;
  return x;
}

// Get total number of bytes used.
static size_t Chunk_count(Chunk *c) {
  if (!c)
    return 0;
  return ((char *)c->alloc - (char *)c->blocks) + Chunk_count(c->prev);
}

// Get total allocated chunk size.
static size_t Chunk_size(Chunk *c) {
  return !c ? 0 : c->size + Chunk_size(c->prev);
}

// Check if chunk contains pointer.
static int Chunk_contains(Chunk *c, void *p) {
  if (!c)
    return 0;
  else if (p >= c->blocks && p < c->alloc)
    return 1;
  return Chunk_contains(c->prev, p);
}

// Check if young heap pointer is below previous collection mark.
// Returns 0 if no previous mark is found/the pointer is not located in the
// given chunk. As such, the latter case should be checked first.
int Chunk_marked(Chunk *c, void *p) {
  if (!c)
    return 0;
  else if (!c->mark)
    return Chunk_marked(c->prev, p);
  else if (p < c->mark && p >= c->blocks)
    return 1;
  return 0;
}

// Generation store two separate singly-linked lists of chunks. The obj list
// stores allocated objects, while the data list stores any managed
// references (strings, vectors, etc.). On collection, obj is scanned using
// breadth-first search.
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

// The heap stores a young (g0), old (g1), and transitory (swap) generation.
// full sweeps occur at fixed intervals, while objects from the young
// generation are promoted to the old generation whenever they survive two
// collections.
struct Heap {
  Generation *g0, *g1, *swap;
  SyState *s;  // roots
  unsigned int minor_gc;  // full sweep counter/flag
};

// Create a new heap with associated state.
Heap *Heap_new(SyState *s) {
  Heap *h = err_malloc(sizeof(Heap));
  h->g0 = Generation_new(HEAP_MIN, HEAP_MIN);
  h->g1 = Generation_new(HEAP_MIN, HEAP_MIN);
  h->swap = Generation_new(HEAP_MIN, HEAP_MIN);
  h->s = s;
  h->minor_gc = FULL_GC_AFTER;
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
    Chunk *c = Chunk_new(HEAP_MIN);
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
    Chunk *c = Chunk_new(MAX(size, HEAP_MIN));
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

// Duplicate a null-terminated string.
char *Heap_strdup(Heap *h, const char *s) {
  return Heap_strndup(h, s, strlen(s));
}

// Duplicate a string of length n.
char *Heap_strndup(Heap *h, const char *s, size_t n) {
  char *d = Heap_malloc(h, n + 1);
  strcpy(d, s);
  d[n] = '\0';
  return d;
}

// Get the number of heap-allocated bytes.
size_t Heap_count(Heap *h) {
  return Generation_count(h->g0) + Generation_count(h->g1);
}

// Get the total heap size, in bytes.
size_t Heap_size(Heap *h) {
  return (Generation_size(h->g0) + 
          Generation_size(h->g1) +
          Generation_size(h->swap));
}

// static int Heap_location(Heap *h, Object *x) {
//   if (Chunk_contains(h->g0->obj, x))
//     return 1;
//   else if (Chunk_contains(h->g1->obj, x))
//     return 2;
//   else
//     return 0;
// }

// ---------------------------------------------------------------------------
// Garbage collection
// ---------------------------------------------------------------------------

// Forward an object to the to-space.
static void forward_object(Heap *h, Object **p) {
  Object *x = *p;
  if (!x)
    return;
  else if (type(x) == T_FWD)  // already forwarded?
    *p = as(x).fwd;
  else if (Chunk_contains(h->swap->obj, x)) {  // is in from-space?
    Object *fwd;
    if (Chunk_marked(h->swap->obj, x))  // copy to old generation
      fwd = Chunk_alloc(&h->g1->obj, sizeof(Object));
    else  // copy to young generation
      fwd = Chunk_alloc(&h->g0->obj, sizeof(Object));

    *fwd = *x;
    type(x) = T_FWD;
    as(x).fwd = fwd;
    *p = fwd;
  }
}

static void copy_data(Heap *h, Object *x);

// Forward a vector to the to-space.
static Vector *forward_vector(Heap *h, Vector **p) {
  Vector *v = *p;
  size_t n = objsize(sizeof(Vector)) + v->size;
  
  // TODO: check location
  // if (Chunk_marked())

  Vector *fwd = Heap_calloc(h, n, sizeof(Object));
  *fwd = *v;

  memcpy(fwd->items, v->items, fwd->len * sizeof(Object));
  for (size_t i = 0; i < fwd->len; i++) {
    copy_data(h, &fwd->items[i]);
  }
  *p = fwd;
}

// Copy and update an object's referenced data.
static void copy_data(Heap *h, Object *x) {
  if (!x)
    return;
  else if (islist(x)) {
    forward_object(h, &car(x));
    forward_object(h, &cdr(x));
  }
  else switch(type(x)) {
    case T_STRING:
      as(x).string = Heap_strdup(h, as(x).string);
      break;
    case T_VECTOR:
    case T_TABLE:
      forward_vector(h, &as(x).vector);
      break;
    default:
      break;
  }
}

static void compact_chunk(Chunk *c, size_t size) {
  Chunk_free(c->prev);
  c->prev = NULL;
  if (c->size < size) {
    c->blocks = err_realloc(c->blocks, size);
    c->size = size;
  }
  c->alloc = c->blocks;
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

  // TODO: collecting old generation (full sweep) 
  
  // set scan/alloc pointers
  Chunk *c0 = h->g0->obj;
  Object *scan = c0->alloc = c0->blocks;

  // copy any references on the stack
  for (size_t i = 0; i < s->top; i++) {
    copy_data(h, &s->stack[i]);
  }

  // scan copied references
  for (; scan != c0->alloc; scan++) {
    copy_data(h, scan);
  }

  if (h->minor_gc)
    h->minor_gc--;
  else
    h->minor_gc = FULL_GC_AFTER;

  h->g0->obj->mark = h->g0->obj->alloc;  // mark end of current gc
  h->g0->data->mark = h->g0->data->alloc;

  // compact swap generation into single chunk
  compact_swap(h);
}
