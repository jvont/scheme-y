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
      printf("out of memory!");
      exit(1);
    }
  }
  s->pin = s->next;
  s->next += n;
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
  // TODO: forward atom contents (symbol/string, vector, etc.)

  cell_t *c = *p;
  if (!c)
    return;
  else if (islist(c)) {
    // TODO: handle strings, vectors, etc.
    if (isfrom(s, c))  // already copied?
      *p = car(c);
    else {
      cell_t *n = s->next++;
      *n = *c;  // copy ref
      car(c) = n;  // place fwd addr at old loc
      *p = n;
    }
  }
  else switch (type(c)) {
    // case T_INT:
    // case T_REAL:
    // case T_CHAR:
    case T_STRING:
    case T_SYMBOL:
      // cellsize(strlen(as(c).string));

    // case T_FFUN:
    case T_VECTOR:
    case T_TABLE:
    // case T_PORT:
    default:
      break;
  }
}

static void forward_atom(SchemeY *s, cell_t **p) {

}

#define move_ref(s,r) (*(s)->next = *(r), (r) = (s)->next++)

// Forward the contents of the global environment.
static void forward_globals(SchemeY *s, vector_t *v) {
  cell_t *cur = v->_items;
  cell_t *end = cur + v->_size;

  cell_t *start = s->next;

  for (; cur < end; cur++) {
    cell_t *next;
    if (car(cur))
      move_ref(s, car(cur));
    if (cdr(cur))
      move_ref(s, cdr(cur));
  }
}

#include "io.h"

void gc(SchemeY *s) {
  printf("before gc: %zu cells\n", s->next - s->heap);

  /* swap semi-spaces */
  cell_t *swap = s->heap;
  s->heap = s->heap2;
  s->heap2 = swap;
  cell_t *scan = s->next = s->heap;
  
  /* forward roots */
  forward_globals(s, s->globals);
  move_ref(s, s->inport);
  move_ref(s, s->outport);

  // TODO: forward environment stack, pin, etc.

  for(; scan != s->next; scan++) {
    printf("forwarding object: ");
    print_obj(scan);
    forward(s, &scan);
  }

  printf("after gc: %zu cells\n", s->next - s->heap);
}
