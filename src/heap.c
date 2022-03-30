#include "heap.h"
#include "runtime.h"
#include "utils.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

struct Chunk {
  Object *blocks, *alloc;
  size_t size;
  Chunk *next;  // next chunk in list
};

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

static int Chunk_contains(Chunk *c, Object *x) {
  if (!c)
    return 0;
  else if (x >= c->blocks && x < c->alloc)
    return 1;
  return Chunk_contains(c->next, x);
}

// Create a new heap with associated state.
Heap *Heap_new(SyState *s) {
  Heap *h = err_malloc(sizeof(Heap));
  h->g0 = Chunk_new(DEFAULT_CHUNK_SIZE);
  h->g1 = Chunk_new(DEFAULT_CHUNK_SIZE);
  h->swap = Chunk_new(DEFAULT_CHUNK_SIZE);
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

// Allocate size bytes of cell-aligned memory.
void *Heap_malloc(Heap *h, size_t size) {
  if (size == 0) return NULL;
  size_t n = objsize(size);
  Chunk *g0 = h->g0;
  if (g0->alloc + n >= g0->blocks + g0->size) {
    Chunk *c = Chunk_new(MAX(n, DEFAULT_CHUNK_SIZE));
    c->next = h->g0;
    h->g0 = c;
  }
  Object *p = g0->alloc;
  g0->alloc += n;
  return p;
}

// Get an array of n objects of size bytes, setting their bytes to zero.
void *Heap_calloc(Heap *h, size_t n, size_t size) {
  void *p = Heap_malloc(h, n * size);
  return !p ? NULL : memset(p, 0, n * size);
}

// Duplicate a null-terminated string.
char *Heap_strdup(Heap *h, const char *s) {
  return Heap_strndup(h, s, strlen(s));
}

// Duplicate a string of length n (null character excluded).
char *Heap_strndup(Heap *h, const char *s, size_t n) {
  char *d = Heap_malloc(h, n + 1);
  return strcpy(d, s);
}

// Copy object reference to the to-space.
static void copy_obj(Heap *h, Object **p) {
  Object *x = *p;

  Chunk *to = h->g0;
  if (Chunk_contains(h->swap, x)) {
    *to->alloc = *x;
    *p = to->alloc;
    to->alloc++;
  }

  // if (!x) return;
  // else if (type(x) == T_FWD) {  /* Already copied */
  //   *p = as(x).fwd;
  // }
  // else {
  //   Object *fwd = h->next++;
  //   *fwd = *((Object *)untag(x));
  //   if (islist(x)) {  /* Copy list */
  //     copy_obj(h, &car(fwd));
  //     copy_obj(h, &cdr(fwd));
  //     fwd = (Object *)tag(fwd);
  //   }
  //   else switch (type(x)) {  /* Copy atom */
  //     case T_STRING:
  //     case T_SYMBOL:
  //       as(fwd).string = Heap_strdup(h, as(x).string);
  //       break;
  //     case T_VECTOR:
  //     case T_TABLE:
  //       // TODO: copy vector
  //       break;
  //     default:
  //       break;
  //   }
  //   type(x) = T_FWD;
  //   as(x).fwd = fwd;
  //   *p = fwd;
  // }
}

void Heap_collect(Heap *h) {
  SyState *s = h->s;

  // swap young generation
  Chunk *swap = h->g0;
  h->g0 = h->swap;
  h->swap = swap;

  // copy roots
  // TODO: figure out what roots look like
  // TODO: scan pointer + chunks issue

  for (size_t i = 0; i < s->top; i++) {
    copy_obj(h, s->stack + i);
  }

  // reset old generation (TODO: for all chunks)
  h->swap->alloc = h->swap->blocks;
}
