#include "heap.h"

#include <stdlib.h>
#include <string.h>

#define isfrom(c) ((c) >= heap && (c) < heap + semi)

/* Create a managed heap, with an associated state. */
Heap *Heap_new() {
  Heap *h = malloc(sizeof(Heap));
  h->from = malloc(2 * HEAP_SIZE * sizeof(Object));
  h->to = h->from + HEAP_SIZE;
  h->size = HEAP_SIZE;
  return h;
}

void Heap_free(Heap *h) {
  free(h->from < h->to ? h->from : h->to);
  free(h);
}

/* Allocate size bytes of cell-aligned memory. */
void *Heap_malloc(Heap *h, size_t size) {
  if (size == 0) return NULL;
  size_t n = objsize(size);
  if (h->next + n >= h->from + h->size) {
    // Heap_resize(s);
  }
  Object *p = h->next;
  h->next += n;
  // printf("  heap: %zu cells\n", heap_next - heap);
  return p;
}

/* Get an array of n objects of size bytes, setting their bytes to zero. */
void *Heap_calloc(Heap *h, size_t n, size_t size) {
  void *p = Heap_malloc(h, n * size);
  return !p ? NULL : memset(p, 0, n * size);
}

/* Duplicate a null-terminated string. */
char *Heap_strdup(Heap *h, const char *s) {
  return Heap_strndup(h, s, strlen(s));
}

/* Duplicate a string of length n (null character excluded). */
char *Heap_strndup(Heap *h, const char *s, size_t n) {
  char *d = Heap_malloc(h, n + 1);
  return strcpy(d, s);
}

/* Copy object reference to the to-space. */
static void copy_obj(Heap *h, Object **p) {
  Object *x = *p;
  if (!x) return;
  else if (isfwd(x)) {  /* Already copied */
    *p = as(x).fwd;
  }
  else {
    Object *fwd = h->next++;
    *fwd = *((Object *)untag(x));
    if (islist(x)) {  /* Copy list */
      copy_obj(h, &car(fwd));
      copy_obj(h, &cdr(fwd));
      fwd = (Object *)tag(fwd);
    }
    else switch (type(x)) {  /* Copy atom */
      case T_STRING:
      case T_SYMBOL:
        as(fwd).string = Heap_strdup(h, as(x).string);
        break;
      case T_VECTOR:
      case T_TABLE:
        // TODO: copy vector
        break;
      default:
        break;
    }
    type(x) = T_FWD;
    as(x).fwd = fwd;
    *p = fwd;
  }
}

/* Collect garbage for a given state. */
void Heap_collect(Heap *h) {
  // printf("before gc: %zu cells\n", heap_next - heap);

  Object *swap = h->from;
  h->from = h->next = h->to;
  h->to = swap;

  // for (size_t i = 0; i < roots_len; i++) {
  //   copy_obj(h, roots[i]);
  // }

  // printf("after gc: %zu cells\n", heap_next - heap);
}
