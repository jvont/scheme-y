#include "scheme-y.h"
#include "object.h"
#include "base.h"
#include "reader.h"
#include "state.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  SchemeY s;
  syS_init(&s);

  if (argc == 1) {  // start REPL
    for (;;) {
      cell *expr = syR_read(&s, NULL);
      // cell *res = eval(expr, e);

      if (expr) {
        printf("=> ");
        print_expr(expr);
        fputc('\n', stdout);
      }
    }
  }
  // else if (argc == 2) {  // open file (TODO: multiple files)
  //   FILE *fp = fopen(argv[1], "r");
  //   if (fp == NULL) {
  //     perror("Cannot open file");
  //     return 1;
  //   }
  //   SchemeY s;
  //   cell port = {
  //     .kind = TyPort,
  //     .marked = true,
  //     .as.port = {.stream = fp, .mode = "r"}
  //   };
  //   cell *expr = syR_read(&s, &port);
  //   // cell *res = eval(expr, e);
  //   if (expr) {
  //     printf("=> ");
  //     print_expr(expr);
  //     fputc('\n', stdout);
  //   }
  //   fclose(fp);
  // }
  else {
    printf("Unknown number of arguments.\n");
    return 1;
  }
  return 0;
}
