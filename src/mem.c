#include "mem.h"
#include "state.h"

#include <stdio.h>
#include <string.h>

#define cellsize(s) ((s + sizeof(cell_t) - 1) / sizeof(cell_t))
#define isfrom(s,c) ((c) >= (s)->heap && (c) < (s)->heap + (s)->semi)

// Allocate a single cell.
cell_t *obj_alloc(SchemeY *s) {
  return heap_malloc(s, sizeof(cell_t));
}

// Allocate size bytes of cell-aligned memory.
void *heap_malloc(SchemeY *s, size_t size) {
  if (!size) return NULL;
  size_t n = cellsize(size);
  if (s->next + n >= s->heap + s->semi) {
    gc(s);
    if (s->next + n >= s->heap + s->semi) {
      // syH_resize(s);
      printf("out of memory!\n");
      exit(1);
    }
  }
  s->pin = s->next;
  s->next += n;
  // printf("  heap: %zu cells\n", s->next - s->heap);
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

// Copy a reference to head and leave behind its forward address.
void set_fwd(SchemeY *s, cell_t *c) {
  *s->next = *c;
  type(c) = T_FWD;
  as(c).fwd = s->next++;
}

// Copy object reference to the to-space.
static void copy_obj(SchemeY *s, cell_t **p) {
  cell_t *c = *p;
  if (!c) return;
  else if (isfwd(c)) {  // already copied
    *p = as(c).fwd;
  }
  else {
    cell_t *fwd = s->next++;
    *fwd = *c;
    if (islist(c)) {  // recursively copy list
      copy_obj(s, &car(fwd));
      copy_obj(s, &cdr(fwd));
    }
    else switch (type(c)) {
      case T_STRING:
      case T_SYMBOL:
        as(fwd).string = heap_strdup(s, as(c).string);
        break;
      case T_VECTOR:
      case T_TABLE:
        // TODO: copy vector
        break;
      default:
        break;
    }
    as(c).fwd = fwd;
    type(c) = T_FWD;
    *p = fwd;
  }
}

#include "io.h"

void gc(SchemeY *s) {
  printf("before gc: %zu cells\n", s->next - s->heap);
  printf("pin: ");
  print_obj(s->pin);

  /* swap semi-spaces */
  cell_t *swap = s->heap;
  s->heap = s->heap2;
  s->heap2 = swap;
  s->next = s->heap;
  
  /* forward roots */
  copy_obj(s, &s->inport);
  copy_obj(s, &s->outport);

  cell_t *cur = s->globals->items;
  cell_t *end = cur + s->globals->size;
  for (; cur < end; cur++) {
    copy_obj(s, &car(cur));
    copy_obj(s, &cdr(cur));
  }

  copy_obj(s, &s->pin);

  printf("after gc: %zu cells\n", s->next - s->heap);
}
