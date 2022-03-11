#include "mem.h"
#include "state.h"

#include <stdio.h>
#include <string.h>

#define isfrom(s,c) ((c) >= (s)->heap && (c) < (s)->heap + (s)->semi)

// Allocate a single cell.
cell_t *obj_alloc(SchemeY *s) {
  return heap_malloc(s, sizeof(cell_t));
}

// Allocate size bytes of cell-aligned memory.
void *heap_malloc(SchemeY *s, size_t size) {
  if (!size) return NULL;
  size_t n = (size + sizeof(cell_t) - 1) / sizeof(cell_t);
  if (s->alloc + n >= s->heap + s->semi) {
    gc(s);
    if (s->alloc + n >= s->heap + s->semi) {
      // syH_resize(s);
      printf("out of memory!");
      exit(1);
    }
  }
  s->pin = s->alloc;
  s->alloc += n;
  return s->pin;
}

// Get an array of n objects of size bytes, setting their bytes to zero.
void *heap_calloc(SchemeY *s, size_t n, size_t size) {
  void *p = heap_malloc(s, n * size);
  return !p ? NULL : memset(p, 0, n * size);
}

// Duplicate a null-terminated string.
char *heap_strdup(SchemeY *s, char *src) {
  return heap_strndup(s, src, strlen(src));
}

// Duplicate a string of length n (null character excluded).
char *heap_strndup(SchemeY *s, char *src, size_t n) {
  char *dest = heap_malloc(s, n + 1);
  return strcpy(dest, src);
}

// Dynamically resize the heap
// static void resize(heap_t *h, size_t size) {

// }

// Forward references to the to-space.
static void forward(SchemeY *s, cell_t **p) {
  cell_t *c = *p;
  if (!c || isatom(c)) return;
  if (iscons(c)) {
    // TODO: handle strings, vectors, etc.
    if (isfrom(s, c))  // already copied?
      *p = car(c);
    else {
      cell_t *n = s->alloc++;
      *n = *c;  // copy ref
      car(c) = n;  // place fwd addr at old loc
      *p = n;
    }
  }
}

static void forward_vector(SchemeY *s, vector_t *v) {
  cell_t *cur = v->_items;
  cell_t *end = cur + v->_size;
  while (cur < end) {
    cell_t *next;
    if (car(cur)) {
      next = s->alloc++;
      *next = *car(cur);
      car(cur) = next;
    }
    if (cdr(cur)) {
      next = s->alloc++;
      *next = *cdr(cur);
      cdr(cur) = next;
    }
    cur++;
  }
}

void gc(SchemeY *s) {
  cell_t *swap = s->heap;
  s->heap = s->heap2;
  s->heap2 = swap;

  cell_t *scn = s->alloc = s->heap;
  
  /* forward roots */
  forward_vector(s, s->globals);
  // TODO: forward environment stack, pin, etc.

  /* forward remaining */
  for(; scn != s->alloc; scn++)
    forward(s, &scn);
}
