#include "builtins.h"
#include "io.h"
#include "mem.h"
#include "object.h"
#include "state.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  SchemeY ss;
  SchemeY *s = &ss;
  sy_init(s);

  sy_intern_bind(s, "+", mk_ffun(s, sy_add));
  sy_intern_bind(s, "x", mk_int(s, 42));

  if (argc == 1) {  // start REPL
    s->prompt = 1;
    for (;;) {
      s->err = E_OK;
      
      cell_t *expr = sy_read(s, NULL);
      cell_t *ret = sy_eval(s, expr);
      printf("=> ");
      print_obj(ret);
    }
  }
  // else if (argc == 2) {  // open file (TODO: multiple files)
  //   FILE *fp = fopen(argv[1], "r");
  //   if (!fp) {
  //     perror("Cannot open file");
  //     return 1;
  //   }
  //   SchemeY s;
  //   cell_t port = {
  //     .kind = TyPort,
  //     .marked = true,
  //     .as.port = {.stream = fp, .mode = "r"}
  //   };
  //   cell_t *expr = syR_parse(&s, &port);
  //   // cell_t *res = eval(expr, e);
  //   if (expr) {
  //     printf("=> ");
  //     write_obj(expr);
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
