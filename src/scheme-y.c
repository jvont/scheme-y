#include "builtins.h"
#include "runtime.h"

#include <stdio.h>
#include <stdlib.h>

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

  s->input = s->i = "(+ 1 2)";
  next(s);

  while (1) {
    // if (argc > 1)
    //   next(e->s);
    // else
    //   input(e->s, "> ");

    Object *obj = eval(e);
    if (obj == NULL)
      break;

    // printf("=> ");
    // print_expr(obj);
    // fputc('\n', stdout);
  }
  return 0;
}
