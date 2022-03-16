#include "mem.h"
#include "state.h"

#include <stdio.h>
#include <string.h>

#define isfrom(s,c) ((c) >= (s)->heap && (c) < (s)->heap + (s)->semi)

// Allocate a single cell.
cell *obj_alloc(SchemeY *s) {
  return heap_malloc(s, sizeof(cell));
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
  cell *c = s->next;
  s->next += n;
  // printf("  heap: %zu cells\n", s->next - s->heap);
  return c;
}

// Get an array of n objects of size bytes, setting their bytes to zero.
void *heap_calloc(SchemeY *s, size_t n, size_t size) {
  void *p = heap_malloc(s, n * size);
  return !p ? NULL : memset(p, 0, n * size);
}

// Copy object reference to the to-space.
static void copy_obj(SchemeY *s, cell **p) {
  cell *c = *p;
  if (!c) return;
  else if (isfwd(c)) {  // already copied
    *p = as(c).fwd;
  }
  else {
    cell *fwd = s->next++;
    *fwd = *c;
    if (islist(c)) {  // recursively copy list
      copy_obj(s, &car(fwd));
      copy_obj(s, &cdr(fwd));
    }
    else switch (type(c)) {
      case T_STRING:
      case T_SYMBOL: {
        size_t n = strlen(as(c).string) - sizeof(size_t) + 1;
        char *dest = heap_malloc(s, n);
        strcpy(as(fwd).string, as(c).string);
      }
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

  /* swap semi-spaces */
  cell *swap = s->heap;
  s->heap = s->heap2;
  s->heap2 = swap;
  s->next = s->heap;
  
  /* forward roots */
  copy_obj(s, &s->inport);
  copy_obj(s, &s->outport);

  cell *cur = s->globals->items;
  cell *end = cur + s->globals->size;
  for (; cur < end; cur++) {
    copy_obj(s, &car(cur));
    copy_obj(s, &cdr(cur));
  }

  copy_obj(s, &s->acc);

  printf("after gc: %zu cells\n", s->next - s->heap);
}
