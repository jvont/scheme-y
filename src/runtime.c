#include "runtime.h"
#include "heap.h"
#include "utils.h"

#include <stdlib.h>

SyState *SyState_new() {
  SyState *s = err_malloc(sizeof(SyState));

  return s;
}

void SyState_free(SyState *s) {

  free(s);
}
