#include "write.h"
#include "state.h"

#include <stdio.h>

static void write_obj(cell *obj, FILE *stream);

static void write_list(cell *obj, FILE *stream) {
  write_obj(car(obj), stream);
  if (cdr(obj)) {
    if (cdr(obj)->kind == TyPair) {
      fputc(' ', stream);
      write_list(cdr(obj), stream);
    }
    else {
      fprintf(stream, " . ");
      write_obj(cdr(obj), stream);
    }
  }
}

static void write_vector(cell *obj, FILE *stream) {
  cell *v = obj->as.vector.items;
  unsigned int len = obj->as.vector.length;
  for (unsigned int i = 0; i < len - 1; i++) {
    write_obj(v + i, stream);
    fputc(' ', stream);
  }
  write_obj(v + len - 1, stream);
}

static void write_obj(cell *obj, FILE *stream) {
  if (obj == NULL)
    fprintf(stream, "()");
  else switch (obj->kind) {
    case TyNil:
      fprintf(stream, "()");
      break;
    case TyInteger:
      fprintf(stream, "%ld", obj->as.integer);
      break;
    case TyReal:
      fprintf(stream, "%.3f", obj->as.real);
      break;
    // case TyRational:
    // case TyComplex:
    case TyBoolean:
      fprintf(stream, obj->as.integer ? "#t" : "#f");
      break;
    case TyCharacter:
      fprintf(stream, "%c", obj->as.character);
    case TyString:
      fprintf(stream, "\"%s\"", obj->as.string);
      break;
    case TySymbol:
      fprintf(stream, "%s", obj->as.string);
      break;
    case TyFFun:
      fprintf(stream, "<foreign-func>");
      break;
    case TyClosure:
      fprintf(stream, "<closure>");
      // write_obj(obj->as.closure.body, stream);
      break;
    case TyPair:
      fputc('(', stream);
      write_list(obj, stream);
      fputc(')', stream);
      break;
    case TyVector: {
      fprintf(stream, "#(");
      write_vector(obj, stream);
      fputc(')', stream);
      break;
    }
    case TyTable:
      fprintf(stream, "#(");
      write_vector(obj, stream);
      fputc(')', stream);
      break;
    case TyPort:
      fprintf(stream, "<port-%s at %p>", obj->as.port.mode, obj->as.port.stream);
      break;
  }
}

// Print an object to stdout.
void syW_print(cell *obj) {
  write_obj(obj, stdout);
  fputc('\n', stdout);
}

// Write an object to given/default output port.
cell *sy_write(SchemeY *s, cell *args) {
  cell *port = cdr(args) ? car(cdr(args)) : s->output_port;
  FILE *stream = port->as.port.stream;
  write_obj(car(args), stream);
  fputc('\n', stream);
  return NULL;
}
