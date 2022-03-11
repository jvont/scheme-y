#include "mem.h"
#include "state.h"

#include <stdio.h>
#include <string.h>

#define isfrom(s,c) ((c) >= (s)->heap && (c) < (s)->heap + (s)->semi)

// Allocate a single cell.
cell_t *obj_alloc(SchemeY *s) {
  return heap_malloc(s, sizeof(cell_t));
}

// Get an array of n objects of size bytes, setting their bytes to zero.
void *obj_calloc(SchemeY *s, size_t n, size_t size) {
  void *p = heap_malloc(s, n * size);
  return !p ? NULL : memset(p, 0, n * size);
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

}

void gc(SchemeY *s) {
  cell_t *swap = s->heap;
  s->heap = s->heap2;
  s->heap2 = swap;

  cell_t *scn = s->alloc = s->heap;
  
  /* forward global refs */
  cell_t *items = s->globals->_items;
  size_t size = s->globals->_size;
  for (size_t i = 0; i < size; i++) {
    cell_t *c = items + i;
    if (iscons(c)) {
      // TODO: replace with forward_vector subroutine
      forward(s, &car(c));
      forward(s, &cdr(c));
    }
  }

  /* forward remaining */
  for(; scn != s->alloc; scn++)
    forward(s, &scn);
}
