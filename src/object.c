#include "object.h"
#include "state.h"
#include "mem.h"

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------------------------
// Initialization
// ---------------------------------------------------------------------------

cell_t *syO_integer(SchemeY *s, long integer) {
  cell_t *p = syM_alloc(s);
  sett(p, INTEGER);
  getv(p).integer = integer;
  return p;
}

cell_t *syO_real(SchemeY *s, double real) {
  cell_t *p = syM_alloc(s);
  sett(p, REAL);
  getv(p).real = real;
  return p;
}

cell_t *syO_character(SchemeY *s, int character) {
  cell_t *p = syM_alloc(s);
  sett(p, CHARACTER);
  getv(p).character = character;
  return p;
}

cell_t *syO_string(SchemeY *s, char *string) {
  cell_t *p = syM_alloc(s);
  sett(p, STRING);
  getv(p).string = syM_strdup(s, string);
  return p;
}

cell_t *syO_symbol(SchemeY *s, char *symbol) {
  cell_t *p = syM_alloc(s);
  sett(p, SYMBOL);
  getv(p).string = syM_strdup(s, symbol);
  return p;
}

cell_t *syO_proc(SchemeY *s, proc_t *proc) {
  cell_t *p = syM_alloc(s);
  sett(p, PROC);
  getv(p).proc = proc;
  return p;
}

cell_t *syO_cons(SchemeY *s, cell_t *car, cell_t *cdr) {
  cell_t *p = syM_alloc(s);
  car(p) = car;
  cdr(p) = cdr;
  return p;
}

cell_t *syO_vector(SchemeY *s, unsigned int size) {
  cell_t *p = syM_alloc(s);
  sett(p, VECTOR);
  getv(p).vector = syM_malloc(s, sizeof(vector_t));
  getv(p).vector->items = syM_calloc(s, size, sizeof(cell_t));
  getv(p).vector->len = 0;
  getv(p).vector->size = size;
  return p;
}

cell_t *syO_table(SchemeY *s, unsigned int size) {
  cell_t *p = syM_alloc(s);
  sett(p, VECTOR);
  getv(p).vector = syM_malloc(s, sizeof(vector_t));
  getv(p).vector->items = syM_calloc(s, size, sizeof(cell_t));
  getv(p).vector->len = 0;
  getv(p).vector->size = size;
  // for (unsigned int i = 1; i < size + 1; i++)
  //   car(getv(p).vector->items + i) = NIL;
  return p;
}

cell_t *syO_port(SchemeY *s, FILE *stream, char *mode) {
  cell_t *p = syM_alloc(s);
  sett(p, PORT);
  getv(p).port = stream;
  return p;
}

// ---------------------------------------------------------------------------
// Read
// ---------------------------------------------------------------------------

static int isdelim(int c) { return isspace(c) || c == '(' || c == ')' || c == ';'; }
static int isinitial(int c) { return isalpha(c) || strchr("!$%&*/:<=>?@^_~", c); }
static int issubseq(int c) { return isinitial(c) || isdigit(c) || strchr("+-.", c); }

#define save_until(r,pred) while (!pred(r->ch)) { save_next(r); } r->buf[r->bp] = '\0'
#define skip_while(r,pred) while (pred(r->ch)) next(r)

#define save_next(r) (save(r, r->ch), next(r))

typedef struct SyReader {
  FILE *stream;  // input stream
  int ch;  // lookahead character
  char buf[BUFSIZ];  // token buffer
  size_t bp;  // token buffer position
  size_t lineno;  // current line number
  size_t depth;  // nested expression depth
  enum {  // read status
    ErrOK, ErrEOF, ErrInvalid, ErrDotsep
  } err;
  SchemeY *s;
} SyReader;

// Get user input, printing prompt to stdout.
static void input_prompt(SyReader *r) {
  if (r->depth) {  // print prompt based on nesting
    for (size_t i = 0; i <= r->depth; i++)
      putc('.', stdout);
  }
  else putc('>', stdout);
  putc(' ', stdout);
  fflush(stdout);
  r->ch = fgetc(r->stream);
  if (r->ch == '\n')  // prompt until non-empty
    input_prompt(r);
}

// Get the next character from file stream/interactive prompt.
static void next(SyReader *r) {
  if (r->stream == stdin) {  // prompt
    if (r->ch == '\n')
      input_prompt(r);
    else
      r->ch = fgetc(r->stream);
  }
  else {  // file
    r->ch = fgetc(r->stream);
    r->lineno += (r->ch == '\n');
  }
}

// Save character to token buffer, resize buffer if needed.
static void save(SyReader *r, int c) {
  if (r->bp < sizeof(r->buf) - 1) {
    r->buf[r->bp++] = c;
    r->buf[r->bp] = '\0';
  }
}

// Save case-insensitive character to token buffer (identifier).
static void save_lower(SyReader *r, int c) {
  if (r->bp < sizeof(r->buf) - 1) {
    r->buf[r->bp++] = tolower(c);
    r->buf[r->bp] = '\0';
  }
}

// Scan an invalid token until next delimiter.
static cell_t *parse_invalid(SyReader *r) {
  r->err = ErrInvalid;
  save_until(r, isdelim);
  return NULL;
}

// Number = integer | real ;
static cell_t *parse_number(SyReader *r) {
  save_until(r, isdelim);
  char *nptr = r->buf;
  char *endptr;
  long i = strtol(nptr, &endptr, 0);
  if (errno == ERANGE || *endptr != '\0') {
    double d = strtod(nptr, &endptr);
    if (errno == ERANGE || *endptr != '\0')
      return parse_invalid(r);  // ERROR: number too large
    else
      return syO_real(r->s, d);
  }
  return syO_integer(r->s, i);
}

// String = " { ( character | escape ) - " } " ;
static cell_t *parse_string(SyReader *r) {
  next(r);
  while (r->ch != '"') {
    if (r->ch == '\\')  // string escape
      save_next(r);
    else if (r->ch == EOF) {  // unexpected EOF
      r->err = ErrEOF;
      break;
    }
    save_next(r);
  }
  next(r);
  return syO_string(r->s, r->buf);
}

// Identifier = initial { subseq } ;
static cell_t *parse_identifier(SyReader *r) {
  while (issubseq(r->ch)) {
    save_lower(r, r->ch);
    next(r);
  }
  r->buf[r->bp] = '\0';
  if (isdelim(r->ch)) {
    return syS_intern(r->s, r->buf);
  }
  else return parse_invalid(r);
}

static cell_t *parse_expr(SyReader *r);

// Parse a list expression.
static cell_t *parse_list(SyReader *r) {
  skip_while(r, isspace);  // skip whitespace
  if (r->ch == ')') {
    r->depth--;
    next(r);
    return NULL;
  }
  bool dotsep = (r->ch == '.');
  cell_t *obj = parse_expr(r);
  cell_t *rest = parse_list(r);
  if (dotsep && rest)
    r->err = ErrDotsep;
  return dotsep ? obj : syO_cons(r->s, obj, rest);
}

// Parse an expression.
static cell_t *parse_expr(SyReader *r) {
  r->bp = 0;
  skip_while(r, isspace);  // skip whitespace
  if (r->ch == '(') {  // list (common)
    r->depth++;
    next(r);
    return parse_list(r);
  }
  else if (isinitial(r->ch))  // identifier
    return parse_identifier(r);
  else if (isdigit(r->ch))  // number
    return parse_number(r);
  else switch (r->ch) {
    case '+': case '-':
      save_next(r);
      if (r->ch == '.' || isdigit(r->ch))  // ( + | - ) [ . ] { digit }
        return parse_number(r);
      else if (isdelim(r->ch))  // ( + | - )
        return parse_identifier(r);
      break;
    case '.':
      save_next(r);
      if (isdelim(r->ch))  // dot separator (if list)
        return (r->depth) ? parse_expr(r) : parse_invalid(r);
      else if (isinitial(r->ch) || r->ch == '.')  // .. { subseq }
        return parse_identifier(r);
      else if (isdigit(r->ch))  // . { digit }
        return parse_number(r);
      break;
    case '"':
      return parse_string(r);  // " { char | esc } "
    case '\'':
      next(r);
      return syO_cons(r->s, syS_intern(r->s, "quote"), syO_cons(r->s, parse_expr(r), NULL));
    case '`':
      next(r);
      return syO_cons(r->s, syS_intern(r->s, "quasiquote"), syO_cons(r->s, parse_expr(r), NULL));
    case ',':
      next(r);
      if (r->ch == '@') {
        next(r);
        return syO_cons(r->s, syS_intern(r->s, "unquote-splicing"), syO_cons(r->s, parse_expr(r), NULL));
      }
      else return syO_cons(r->s, syS_intern(r->s, "unquote"), syO_cons(r->s, parse_expr(r), NULL));
    case '#':
      next(r);
      switch (r->ch) {
        case 't':
          next(r);
          return syS_intern(r->s, "#t");
        case 'f':
          next(r);
          return syS_intern(r->s, "#f");
        case '|':  // block comment #| ... |#
        case '\\':  // character literal (handle special cases)
        case '(':  // vector literal
        case 'e': case 'i': case 'o': case 'd': case 'x':  // number literal
        case 'u':  // bytevector constant, #u8(
        default:  // datum label, #<digits>= #<digits>#
          break;
      }
      break;
    case EOF:
      r->err = ErrEOF;
      return NULL;
    default:
      break;
  }
  return parse_invalid(r);
}

static const char *err_msgs[] = {
  [ErrOK] = "",
  [ErrEOF] = "unexpected EOF",
  [ErrInvalid] = "invalid token",
  [ErrDotsep] = "invalid token following dot separator",
};

static void reader_init(SchemeY *s, SyReader *r, cell_t *port) {
  r->stream = getv(port).port;
  r->ch = (r->stream == stdin) ? '\n' : EOF;
  r->bp = 0;
  r->lineno = 1;
  r->depth = 0;
  r->err = ErrOK;
  r->s = s;
}

static void read_error(SyReader *r) {
  printf("Syntax Error: %s\n", err_msgs[r->err]);
  printf("Line %zu, at \"%s\"\n", r->lineno, r->buf);
}

// Read the next expression.
cell_t *syO_read(SchemeY *s, cell_t *arg) {
  SyReader r;
  reader_init(s, &r, arg ? car(arg) : s->input_port);
  next(&r);
  cell_t *expr = parse_expr(&r);
  if (r.ch == EOF)
    return syS_intern(s, "#!eof");
  else if (r.err) {
    read_error(&r);
    return NULL;
  }
  // add expression to heap/stack
  return expr;
}

// ---------------------------------------------------------------------------
// Write
// ---------------------------------------------------------------------------

static void write_obj(cell_t *obj, FILE *stream);

static void write_list(cell_t *obj, FILE *stream) {
  write_obj(car(obj), stream);
  if (cdr(obj)) {
    if (iscons(cdr(obj))) {
      fputc(' ', stream);
      write_list(cdr(obj), stream);
    }
    else {
      fprintf(stream, " . ");
      write_obj(cdr(obj), stream);
    }
  }
}

static void write_vector(cell_t *obj, FILE *stream) {
  cell_t *items = getv(obj).vector->items;
  unsigned int len = getv(obj).vector->len;
  for (unsigned int i = 0; i < len - 1; i++) {
    write_obj(items + i, stream);
    fputc(' ', stream);
  }
  write_obj(items + len - 1, stream);
}

static void write_obj(cell_t *obj, FILE *stream) {
  if (!obj)
    fprintf(stream, "()");
  else if (iscons(obj)) {
    fputc('(', stream);
    write_list(obj, stream);
    fputc(')', stream);
  }
  else switch (gett(obj)) {
    case NIL:
      fprintf(stream, "()");
      break;
    case INTEGER:
      fprintf(stream, "%ld", getv(obj).integer);
      break;
    case REAL:
      fprintf(stream, "%.3f", getv(obj).real);
      break;
    // case RATIONAL:
    // case COMPLEX:
    // case BOOLEAN:
    //   fprintf(stream, getv(obj).integer ? "#t" : "#f");
    //   break;
    case CHARACTER:
      fprintf(stream, "%c", getv(obj).character);
      break;
    case STRING:
      fprintf(stream, "\"%s\"", getv(obj).string);
      break;
    case SYMBOL:
      fprintf(stream, "%s", getv(obj).string);
      break;
    case PROC:
      fprintf(stream, "<builtin-procedure>");
      break;
    case CLOSURE:
      fprintf(stream, "<closure>");
      // write_obj(getv(obj).closure.body, stream);
      break;
    case VECTOR: {
      fprintf(stream, "#(");
      write_vector(obj, stream);
      fputc(')', stream);
      break;
    }
    case TABLE:
      fprintf(stream, "#(");
      write_vector(obj, stream);
      fputc(')', stream);
      break;
    case PORT:
      fprintf(stream, "<port>");
      break;
  }
}

// Print an object to stdout.
void syO_print(cell_t *obj) {
  write_obj(obj, stdout);
  fputc('\n', stdout);
}

// Write an object to given/default output port.
cell_t *syO_write(SchemeY *s, cell_t *args) {
  cell_t *port = cdr(args) ? car(cdr(args)) : s->output_port;
  FILE *stream = getv(port).port;
  write_obj(car(args), stream);
  fputc('\n', stream);
  return NULL;
}
