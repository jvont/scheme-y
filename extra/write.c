static void write_obj(Object *obj, FILE *stream);

static void write_list(Object *obj, FILE *stream) {
  write_obj(car(obj), stream);
  if (cdr(obj)) {
    if (islist(cdr(obj))) {
      fputc(' ', stream);
      write_list(cdr(obj), stream);
    }
    else {
      fprintf(stream, " . ");
      write_obj(cdr(obj), stream);
    }
  }
}

static void write_vector(Object *obj, FILE *stream) {
  Object *items = as(obj).vector->items;
  size_t len = as(obj).vector->len;
  for (size_t i = 0; i < len - 1; i++) {
    write_obj(items + i, stream);
    fputc(' ', stream);
  }
  write_obj(items + len - 1, stream);
}

// TODO: handle quote, quasiquote, unquote, unquote-splicing, vector, etc.
static void write_obj(Object *obj, FILE *stream) {
  if (!obj)
    fprintf(stream, "()");
  else if (islist(obj)) {
    fputc('(', stream);
    write_list(obj, stream);
    fputc(')', stream);
  }
  else switch (type(obj)) {
    case T_INTEGER:
      fprintf(stream, "%d", as(obj).integer);
      break;
    case T_REAL:
      if (as(obj).real < 1e-3 || as(obj).real > 1e6)
        fprintf(stream, "%.3e", as(obj).real);
      else
        fprintf(stream, "%.3f", as(obj).real);
      break;
    case T_BOOLEAN:
      fprintf(stream, as(obj).integer ? "#t" : "#f");
      break;
    case T_CHARACTER:
      fprintf(stream, "%c", as(obj).character);
      break;
    case T_STRING:
      fprintf(stream, "\"%s\"", as(obj).string);
      break;
    case T_SYMBOL:
      fprintf(stream, "%s", as(obj).string);
      break;
    case T_CCLOSURE:
      fprintf(stream, "<foreign-func>");
      break;
    case T_VECTOR:
      fprintf(stream, "#(");
      write_vector(obj, stream);
      fputc(')', stream);
      break;
    case T_TABLE:
      fprintf(stream, "#(");
      write_vector(obj, stream);
      fputc(')', stream);
      break;
    case T_PORT:
      fprintf(stream, "<port>");
      break;
  }
}

/* Print an object to stdout. */
void print_obj(Object *obj) {
  write_obj(obj, stdout);
  fputc('\n', stdout);
}

/* Write an object to given/default output port. */
Object *sy_write(Object *args) {
  FILE *stream = cdr(args) ? as(car(cdr(args))).port : stdout; // DEFAULT_OUTPUT_PORT;
  write_obj(car(args), stream);
  fputc('\n', stream);
  return NULL;
}