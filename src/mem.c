#include "mem.h"
#include "state.h"

#include <string.h>

// Create a new heap with a half-size of size cells.
Heap *syM_new(size_t size) {
  Heap *h = malloc(sizeof(Heap));
  if (!h) exit(1);
  h->from = h->head = malloc(2 * size * sizeof(cell));
  if (!h->from) exit(1);
  h->to = h->from + size;
  h->size = size;
}

// Free a heap object.
void syM_free(Heap *h) {
  free(h->from < h->to ? h->from : h->to);
  free(h);
}

// Allocate n bytes of memory.
void *syM_alloc(SchemeY *s, size_t n) {
  Heap *h = s->heap;
  if (h->head + n >= h->from + h->size) {
    syM_gc(s);
    if (h->head + n >= h->from + h->size) {
      // syH_resize(s);
      perror("out of memory!");
      exit(1);
    }
  }
  void *p = h->head;
  h->head += n;
  return p;
}

// Get an array of n cells, setting their bytes to zero.
void *syM_calloc(SchemeY *s, size_t n, size_t size) {
  void *p = syM_alloc(s, n * size);
  return memset(p, 0, n * size);
}

// Duplicate a null-terminated string.
char *syM_strdup(SchemeY *s, char *src) {
  return syM_strndup(s, src, strlen(src));
}

// Duplicate a string of length n (null character excluded).
char *syM_strndup(SchemeY *s, char *src, size_t n) {
  char *dest = syM_alloc(s, n + 1);
  return strcpy(dest, src);
}

// Dynamically resize the heap
// static void resize(Heap *h, size_t size) {

// }

static void forward(Heap *h, void **p) {
  // if ((*p)->kind != TyPair) return;

  // void *obj = *p;
  // void *fwd = car(obj);
  
  // if(h->from <= fwd && fwd < h->from + h->size) {
  //   *p = fwd;
  // }
  // else {
  //   void *c = h->head++;
  //   *c = *obj;
  //   car(obj) = c;
  //   *p = c;
  // }
}

void syM_gc(SchemeY *s) {
  Heap *h = s->heap;

  cell *swap = h->from;
  h->from = h->to;
  h->to = swap;

  // cell *scn = h->head = h->from;

  // forward(heap, &(heap->root));
  // for(; scn != heap->head; scn++) {
  //   forward(heap, &(scn->car));
  //   forward(heap, &(scn->cdr));
  // }
}
