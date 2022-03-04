#include "mem.h"
#include "state.h"

#include <stdio.h>
#include <string.h>

// Create a new heap with a half-size of size bytes.
Heap *syM_new(size_t size) {
  Heap *h = malloc(sizeof(Heap));
  if (!h) exit(1);
  h->from = h->next = malloc(2 * size);
  if (!h->from) exit(1);
  h->to = h->from + size;
  h->size = size;
}

// Free a heap object.
void syM_free(Heap *h) {
  free(h->from < h->to ? h->from : h->to);
  free(h);
}

// Allocate a single cell.
cell_t *syM_alloc(SchemeY *s) {
  return syM_malloc(s, sizeof(cell_t));
}

// Allocate size bytes of cell-aligned memory.
void *syM_malloc(SchemeY *s, size_t size) {
  if (!size) return NULL;
  size_t n = (size + sizeof(cell_t) - 1) / sizeof(cell_t);
  Heap *h = s->heap;
  if (h->next + n >= h->from + h->size) {
    syM_gc(s);
    if (h->next + n >= h->from + h->size) {
      // syH_resize(s);
      printf("out of memory!");
      exit(1);
    }
  }
  cell_t *p = h->next;
  h->next += n;
  return p;
}

// Get an array of n objects of size bytes, setting their bytes to zero.
void *syM_calloc(SchemeY *s, size_t n, size_t size) {
  void *p = syM_malloc(s, n * size);
  return !p ? NULL : memset(p, 0, n * size);
}

// Duplicate a null-terminated string.
char *syM_strdup(SchemeY *s, char *src) {
  return syM_strndup(s, src, strlen(src));
}

// Duplicate a string of length n (null character excluded).
char *syM_strndup(SchemeY *s, char *src, size_t n) {
  char *dest = syM_malloc(s, n + 1);
  return strcpy(dest, src);
}

// Dynamically resize the heap
// static void resize(Heap *h, size_t size) {

// }

#define isfrom(h,c) ((c) >= (h)->from && (c) < (h)->from + (h)->size)

// Forward references to the to-space.
static void forward(Heap *h, cell_t **p) {
  cell_t *obj = *p;
  cell_t *next = h->next;
  if (!obj)
    return;
  else if (iscons(obj) || istype(obj, CLOSURE)) {

  }
  else switch (gett(obj)) {
    case STRING:
    case SYMBOL:

      break;
    case VECTOR:
    case TABLE:

      break;
    default:

      break;
  }

  void *fwd = car(obj);
  if (isfrom(h,(cell_t *)fwd))
    *p = fwd;
  else {  // TODO: handle vectors, strings, etc.
    cell_t *next = h->next++;
    *next = *obj;
    car(obj) = next;
    *p = next;
  }
}

void syM_gc(SchemeY *s) {
  Heap *h = s->heap;

  cell_t *swap = h->from;
  h->from = h->to;
  h->to = swap;

  cell_t *scn = h->next = h->from;
  forward(h, &(s->global_env));
  // for(; scn != heap->next; scn++) {
  //   forward(heap, &(scn->car));
  //   forward(heap, &(scn->cdr));
  // }
}
