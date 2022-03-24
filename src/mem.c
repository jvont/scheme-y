#include "mem.h"

// #include <stdio.h>
#include <string.h>

#define isfrom(c) ((c) >= heap && (c) < heap + semi)

Object *heap, *heap2;
Object *heap_next;
size_t semi;

Object ***roots;
size_t roots_len, roots_size;

/* Initialize managed heap for n_generations. */
void mem_init(int n_generations) {
  heap = malloc(2 * HEAP_SIZE * sizeof(Object));
  if (!heap) exit(1);
  heap2 = heap + HEAP_SIZE;
  heap_next = heap;
  semi = HEAP_SIZE;

  roots = malloc(ROOTS_SIZE * sizeof(Object **));
  roots_len = 0;
  roots_size = ROOTS_SIZE;
}

/* Free heap + saved roots. */
void mem_shutdown() {
  free(heap < heap2 ? heap : heap2);
  free(roots);
}

/* Push a new root to the heap. */
void mem_push(Object **root) {
  if (roots_len == roots_size) {
    roots = realloc(roots, roots_size * 2);
    if (!roots) exit(1);
  }
  roots[roots_len++] = root;
}

/* Pop the last root from the heap. */
void mem_pop(Object **root) {
  roots_len--;
}

/* Allocate size bytes of cell-aligned memory. */
void *mem_malloc(size_t size) {
  if (size == 0) return NULL;
  size_t n = objsize(size);
  if (heap_next + n >= heap + semi) {
    garbage_collect(0);
    if (heap_next + n >= heap + semi) {
      // syH_resize(s);
      printf("out of memory!\n");
      exit(1);
    }
  }
  Object *p = heap_next;
  heap_next += n;
  // printf("  heap: %zu cells\n", heap_next - heap);
  return p;
}

/* Get an array of n objects of size bytes, setting their bytes to zero. */
void *mem_calloc(size_t n, size_t size) {
  void *p = mem_malloc(n * size);
  return !p ? NULL : memset(p, 0, n * size);
}

/* Duplicate a null-terminated string. */
char *mem_strdup(const char *s) {
  return mem_strndup(s, strlen(s));
}

/* Duplicate a string of length n (null character excluded). */
char *mem_strndup(const char *s, size_t n) {
  char *d = mem_malloc(n + 1);
  return strcpy(d, s);
}

/* Copy object reference to the to-space. */
static void copy_obj(Object **p) {
  Object *x = *p;
  if (!x) return;
  else if (isfwd(x)) {  // already copied
    *p = as(x).fwd;
  }
  else {
    Object *fwd = heap_next++;
    *fwd = *((Object *)untag(x));
    if (islist(x)) {  // copy list
      copy_obj(&car(fwd));
      copy_obj(&cdr(fwd));
      fwd = (Object *)tag(fwd);
    }
    else switch (type(x)) {  // copy atom
      case T_STRING:
      case T_SYMBOL:
        as(fwd).string = mem_strdup(as(x).string);
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

/* Collect garbage for the specified generation and its children. */
void garbage_collect(int level) {
  // printf("before gc: %zu cells\n", heap_next - heap);
  
  /* swap semi-spaces */
  Object *swap = heap;
  heap = heap2;
  heap2 = swap;
  heap_next = heap;
  
  /* forward roots */
  for (size_t i = 0; i < roots_len; i++) {
    copy_obj(roots[i]);
  }
  
  // printf("after gc: %zu cells\n", heap_next - heap);
}
