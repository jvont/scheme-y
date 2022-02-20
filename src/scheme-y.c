#include "builtins.h"
#include "runtime.h"

#include <stdio.h>
#include <stdlib.h>

// Object *fadd(Object *args) {

// }

int main(int argc, char **argv) {
  FILE *stream = stdin;
  if (argc > 1) {
    FILE *stream = fopen(argv[1], "r");
    if (stream == NULL) {
      perror("Cannot open file");
      return 1;
    }
  }
  Scanner *s = scanner_new();
  s->port = obj_port(stream, "r");

  Env *e = env_new();
  e->s = s;

  while (1) {
    if (argc > 1)
      next(e->s);
    else
      input(e->s, "> ");

    Object *expr = parse(e);
    if (expr == NULL)
      break;

    Object *ret = eval(expr, e);

    printf("=> ");
    print_expr(ret);
    fputc('\n', stdout);
  }
  return 0;
}
