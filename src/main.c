#include "scheme-y.h"
#include "builtins.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc == 1) {  // start REPL
    SchemeY *s = sy_new(stdin);
    for (;;) {
      Obj *expr = sy_parse(s);

      // Obj *res = eval(expr, e);

      if (!s->err) {
        printf("=> ");
        print_expr(expr);
        fputc('\n', stdout);
      }
      else sy_error(s);
    }
    sy_free(s);
  }
  else if (argc == 2) {  // open file
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
      perror("Cannot open file");
      return 1;
    }
    SchemeY *s = sy_new(fp);
    Obj *expr = sy_parse(s);
    // Obj *res = eval(expr, e);

    if (!s->err) {
      printf("=> ");
      print_expr(expr);
      fputc('\n', stdout);
    }
    else sy_error(s);
    fclose(fp);
    sy_free(s);
  }
  else {
    printf("Unknown number of arguments.\n");
    return 1;
  }
  return 0;
}
