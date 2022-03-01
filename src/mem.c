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

// Allocate n bytes of memory.
void *syM_alloc(SchemeY *s, size_t n) {
  Heap *h = s->heap;
  if (h->next + n >= h->from + h->size) {
    syM_gc(s);
    if (h->next + n >= h->from + h->size) {
      // syH_resize(s);
      printf("out of memory!");
      exit(1);
    }
  }
  void *p = h->next;
  h->next += n;
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

#define isfrom(h,c) ((c) >= (h)->from && (c) < (h)->from + (h)->size)

// Forward references to the to-space.
static void forward(Heap *h, cell **p) {
  cell *obj = *p;
  cell *next = h->next;
  if (!obj)
    return;
  else switch (obj->kind) {
    case TyString:
    case TySymbol:

      break;
    case TyClosure:
    case TyPair:

      break;
    case TyVector:
    case TyTable:

      break;
    default:

      break;
  }

  void *fwd = car(obj);
  if (isfrom(h,fwd))
    *p = fwd;
  else {  // TODO: handle vectors, strings, etc.
    cell *next = h->next;
    h->next += sizeof(cell);
    *next = *obj;
    car(obj) = next;
    *p = next;
  }
}

void syM_gc(SchemeY *s) {
  Heap *h = s->heap;

  cell *swap = h->from;
  h->from = h->to;
  h->to = swap;

  cell *scn = h->next = h->from;
  forward(h, &(s->global_env));
  // for(; scn != heap->next; scn++) {
  //   forward(heap, &(scn->car));
  //   forward(heap, &(scn->cdr));
  // }
}
