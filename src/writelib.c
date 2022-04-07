#include "runtime.h"

#include <stdio.h>

static void write_obj(Object *x, FILE *stream);

static void write_list(Object *x, FILE *stream) {
  write_obj(car(x), stream);
  if (cdr(x)) {
    if (islist(cdr(x))) {
      fputc(' ', stream);
      write_list(cdr(x), stream);
    }
    else {
      fprintf(stream, " . ");
      write_obj(cdr(x), stream);
    }
  }
}

static void write_vector(Object *x, FILE *stream) {
  Object *items = as(x).vector->items;
  size_t len = as(x).vector->len;
  for (size_t i = 0; i < len - 1; i++) {
    write_obj(items + i, stream);
    fputc(' ', stream);
  }
  write_obj(items + len - 1, stream);
}

// TODO: handle quote, quasiquote, unquote, unquote-splicing, vector, etc.
static void write_obj(Object *x, FILE *stream) {
  if (!x)
    fprintf(stream, "()");
  else if (islist(x)) {
    fputc('(', stream);
    write_list(x, stream);
    fputc(')', stream);
  }
  else switch (type(x)) {
    case T_INTEGER:
      fprintf(stream, "%d", as(x).integer);
      break;
    case T_REAL:
      if (as(x).real < 1e-3 || as(x).real > 1e6)
        fprintf(stream, "%.3e", as(x).real);
      else
        fprintf(stream, "%.3f", as(x).real);
      break;
    case T_BOOLEAN:
      fprintf(stream, as(x).integer ? "#t" : "#f");
      break;
    case T_CHARACTER:
      fprintf(stream, "%c", as(x).character);
      break;
    case T_STRING:
      fprintf(stream, "\"%s\"", as(x).string);
      break;
    case T_SYMBOL:
      fprintf(stream, "%s", as(x).string);
      break;
    case T_CCLOSURE:
      fprintf(stream, "<foreign-func>");
      break;
    case T_VECTOR:
      fprintf(stream, "#(");
      write_vector(x, stream);
      fputc(')', stream);
      break;
    case T_TABLE:
      fprintf(stream, "#(");
      write_vector(x, stream);
      fputc(')', stream);
      break;
    case T_PORT:
      fprintf(stream, "<port>");
      break;
  }
}

// Write an object to given/default output port.
int write_write(SyState *s) {
  Object *port = s->top == 2 ? &s->stack[--s->top] : NULL;
  if (port != NULL && type(port) != T_PORT)
    return E_RUNTIME;

  FILE *stream = !port ? DEFAULT_OUTPUT_PORT : as(port).port;
  Object *x = &s->stack[--s->top];

  write_obj(x, stream);
  return E_OK;
}
