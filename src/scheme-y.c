#include "scheme-y.h"
#include "object.h"
#include "builtins.h"
#include "state.h"

#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static cell_t *syB_quote(SchemeY *s, cell_t *args) {
  return args;
}

int main(int argc, char **argv) {
  SchemeY s;
  syS_init(&s);

  // cell_t *add = syS_intern(&s, "+");
  // cell_t *addfn = syS_lookup(&s, add);
  // cdr(addfn) = mk_ffun(&s, syB_add);

  // cell_t *quote = syS_intern(&s, "quote");
  // cell_t *qfun = syS_lookup(&s, quote);
  // cdr(qfun) = mk_ffun(&s, syB_quote);

  // cell_t *x = syS_intern(&s, "x");
  // cell_t *xv = syS_lookup(&s, x);
  // cdr(xv) = mk_int(&s, 42);

  if (argc == 1) {  // start REPL
    s.prompt = 1;
    for (;;) {
      s.err = E_OK;
      
      cell_t *expr = sy_read(&s, NULL);
      printf("=> ");
      print_obj(expr);
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
