#include "heap.h"
#include "runtime.h"
#include "utils.h"

#include <stdlib.h>
#include <string.h>

#define isfrom(g,x) ((x) >= (g)->blocks && (x) < (g)->alloc)

// Create a new heap with associated state.
Heap *Heap_new(SyState *s) {
  Heap *h = err_malloc(sizeof(Heap));
  for (size_t i = 0; i <= N_GENERATIONS; i++) {
    Generation *g = h->g0 + i;
    g->blocks = err_malloc(GENERATION_SIZE * sizeof(Object));
    g->alloc = g->blocks;
    g->size = GENERATION_SIZE;
  }
  h->gn = h->g0 + N_GENERATIONS - 1;
  h->s = s;
  return h;
}

void Heap_free(Heap *h) {
  for (size_t i = 0; i <= N_GENERATIONS; i++)
    free(h->g0[i].blocks);
  free(h);
}

// Allocate size bytes of cell-aligned memory.
void *Heap_malloc(Heap *h, size_t size) {
  if (size == 0) return NULL;
  size_t n = objsize(size);
  Generation *g0 = h->g0;
  if (g0->alloc + n >= g0->blocks + g0->size) {
    // Heap_resize(s);
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
  // printf("before gc: %zu cells\n", heap_next - heap);

  SyState *s = h->s;

  Object **p = &car(s->stack);
  Object *x = *p;

  Generation *g = h->g0;
  while (g < h->gn) {
    Generation *g1 = g + 1;
    if (isfrom(g, x)) {
      *g1->alloc = *x;
      *p = g1->alloc;
      g1->alloc++;
      break;
    }
    g = g1;
  }

  // Object *swap = h->from;
  // h->from = h->next = h->to;
  // h->to = swap;

  // for (size_t i = 0; i < roots_len; i++) {
  //   copy_obj(h, roots[i]);
  // }

  // printf("after gc: %zu cells\n", heap_next - heap);
}
