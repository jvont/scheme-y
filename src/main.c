#include "scheme-y.h"

int main(int argc, char **argv) {
  SyState *s = sy_open();

  for (;;) {
    int err = read_read(s);
    if (err) break;
    // print_obj(s->stack);
    SyState_pop(s);
  }

  sy_close(s);

  return 0;
}
