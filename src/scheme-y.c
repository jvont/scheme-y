#include "builtins.h"
#include "scanner.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  FILE *file = NULL;
  Scanner *s;
  if (argc > 1) {
    file = fopen(argv[1], "r");
    if (file == NULL) {
      perror("Cannot open file");
      return 1;
    }
    s = scanner_stream(file);
  }
  else s = scanner_prompt();

  // Env *e = env_new();
  // e->s = s;

  while (1) {
    Object *expr = parse(s);
    if (expr == NULL)
      break;

    // Object *ret = eval(expr, e);

    printf("=> ");
    print_expr(expr);
    fputc('\n', stdout);
  }
  if (file)
    fclose(file);
  return 0;
}
