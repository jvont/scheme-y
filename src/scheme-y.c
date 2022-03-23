// #include "io.h"
#include "mem.h"
#include "object.h"
// #include "state.h"

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {


  // State ss;
  // State *s = &ss;
  // sy_init(s);

  // sy_intern_bind(s, "+", ffun(s, sy_add));
  // sy_intern_bind(s, "x", mk_int(s, 42));

  // if (argc == 1) {  // start REPL
  //   s->prompt = 1;
  //   for (;;) {
  //     s->err = E_OK;

  //     printf("memory: %zu of %zu\n", s->next - s->heap, s->semi);
  //     s->acc = sy_read(s, NULL);
  //     printf("read: ");
  //     print_obj(s->acc);

  //     s->acc = sy_eval(s, s->acc);
  //     // gc(s);

  //     printf("=> ");
  //     print_obj(s->acc);
  //   }
  // }
  // else if (argc == 2) {  // open file (TODO: multiple files)
  //   FILE *fp = fopen(argv[1], "r");
  //   if (!fp) {
  //     perror("Cannot open file");
  //     return 1;
  //   }
  //   State s;
  //   Cell port = {
  //     .kind = TyPort,
  //     .marked = true,
  //     .as.port = {.stream = fp, .mode = "r"}
  //   };
  //   Cell *expr = syR_parse(&s, &port);
  //   // Cell *res = eval(expr, e);
  //   if (expr) {
  //     printf("=> ");
  //     write_obj(expr);
  //     fputc('\n', stdout);
  //   }
  //   fclose(fp);
  // }
  // else {
  //   printf("Unknown number of arguments.\n");
  //   return 1;
  // }
  return 0;
}
