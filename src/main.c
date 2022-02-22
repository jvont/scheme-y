#include "scheme-y.h"
#include "builtins.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc > 1) {  // file
    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
      perror("Cannot open file");
      return 1;
    }

    SchemeY *s = sy_stream(fp);
    
    Object *expr = sy_parse(s);

    if (!s->err) {
      printf("=> ");
      print_expr(expr);
      fputc('\n', stdout);
    }
    else sy_error(s);

    fclose(fp);
  }
  else {  // REPL
    SchemeY *s = sy_prompt();
    for (;;) {
      Object *expr = sy_parse(s);

      // Object *ret = eval(expr, e);

      if (!s->err) {
        printf("=> ");
        print_expr(expr);
        fputc('\n', stdout);
      }
      else sy_error(s);
    }
  }
  return 0;
}
