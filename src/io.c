#include "io.h"

#include "object.h"
#include "state.h"
#include "mem.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int isdelim(int c) { return isspace(c) || c == '(' || c == ')' || c == ';'; }
static int isinitial(int c) { return isalpha(c) || strchr("!$%&*/:<=>?@^_~", c); }
static int issubseq(int c) { return isalnum(c) || strchr("!$%&*/:<=>?@^_~+-.", c); }

#define save_until(s,pred) while (!pred(s->lookahead)) { save_next(s); } *s->tp = '\0'
#define skip_while(s,pred) while (pred(s->lookahead)) next(s)
#define save_next(s) (save(s, s->lookahead), next(s))

// Get user input, printing prompt to stdout.
static void input_prompt(SchemeY *s) {
  if (s->depth) {  // print prompt based on nesting
    for (size_t i = 0; i <= s->depth; i++)
      putc('.', stdout);
  }
  else putc('>', stdout);
  putc(' ', stdout);
  fflush(stdout);
  s->lookahead = fgetc(as(s->inport).port);
  if (s->lookahead == '\n')  // prompt until non-empty
    input_prompt(s);
}

// Get the next character from file/interactive prompt.
static void next(SchemeY *s) {
  if (s->prompt) {  // prompt
    if (s->lookahead == '\n')
      input_prompt(s);
    else
      s->lookahead = fgetc(stdin);
  }
  else {  // file
    s->lookahead = fgetc(as(s->inport).port);
    s->lineno += (s->lookahead == '\n');
  }
}

// cell *sy_read_char(SchemeY *s, cell *port) {

// }

// Save character to token buffer, resize buffer if needed.
static void save(SchemeY *s, int c) {
  if (s->tp >= s->tend) {  // resize
    if (s->tend - s->token >= SIZE_MAX / 2) {
      fprintf(stderr, "maximum buffer size reached\n");
      exit(1);
    }
    size_t newsize = 2 * (s->tend - s->token);
    char *newtbase = realloc(s->token, newsize);
    if (!newtbase) {
      fprintf(stderr, "buffer realloc failed\n");
      exit(1);
    }
    s->tp = newtbase + (s->tp - s->token);
    s->token = newtbase;
    s->tend = s->token + newsize;
  }
  *s->tp++ = c;
  // *s->tp = '\0';
}

// Scan an invalid token until next delimiter.
static cell *parse_invalid(SchemeY *s) {
  s->err = E_TOKEN;
  save_until(s, isdelim);
  return NULL;
}

// Number = integer | real ;
static cell *parse_number(SchemeY *s) {
  save_until(s, isdelim);
  errno = 0;  // reset errno before call
  char *nptr = s->token;
  char *endptr;
  long i = strtol(nptr, &endptr, 0);
  if (errno == ERANGE || *endptr != '\0') {
    float r = strtof(nptr, &endptr);
    if (errno == ERANGE || *endptr != '\0')
      return parse_invalid(s);  // ERROR: number too large
    else
      return mk_real(s, r);
  }
  return mk_int(s, i);
}

// String = " { ( character | escape ) - " } " ;
static cell *parse_string(SchemeY *s) {
  next(s);
  while (s->lookahead != '"') {
    if (s->lookahead == '\\')  // string escape
      save_next(s);
    else if (s->lookahead == EOF) {  // unexpected EOF
      s->err = E_EOF;
      break;
    }
    save_next(s);
  }
  *s->tp = '\0';
  next(s);
  return mk_string(s, s->token);
}

// Identifier = initial { subseq } ;
static cell *parse_identifier(SchemeY *s) {
  while (issubseq(s->lookahead)) {
    save(s, tolower(s->lookahead));
    next(s);
  }
  *s->tp = '\0';
  if (isdelim(s->lookahead))
    return sy_intern(s, s->token);
  else
    return parse_invalid(s);
}

static cell *parse_expr(SchemeY *s);

// Parse a list expression.
static cell *parse_list(SchemeY *s) {
  skip_while(s, isspace);  // skip whitespace
  if (s->lookahead == ')') {
    s->depth--;
    next(s);
    return NULL;
  }
  int dotsep = (s->lookahead == '.');



  cell *obj = parse_expr(s);  // TODO: handle object pinning
  cell *rest = parse_list(s);
  if (dotsep && rest)
    s->err = E_DOTSEP;
  return dotsep ? obj : cons(s, obj, rest);
}

// Parse an expression.
static cell *parse_expr(SchemeY *s) {
  s->tp = s->token;
  skip_while(s, isspace);  // skip whitespace
  if (s->lookahead == '(') {  // list (common)
    s->depth++;
    next(s);
    // TODO: handle illegal dot separator at start of list
    return parse_list(s);
  }
  else if (isinitial(s->lookahead))  // identifier
    return parse_identifier(s);
  else if (isdigit(s->lookahead))  // number
    return parse_number(s);
  else switch (s->lookahead) {
    case '+': case '-':
      save_next(s);
      if (s->lookahead == '.' || isdigit(s->lookahead))  // ( + | - ) [ . ] { digit }
        return parse_number(s);
      else if (isdelim(s->lookahead))  // ( + | - )
        return parse_identifier(s);
      break;
    case '.':
      save_next(s);
      if (isdelim(s->lookahead))  // dot separator (if list)
        return (s->depth) ? parse_expr(s) : parse_invalid(s);
      else if (isinitial(s->lookahead) || s->lookahead == '.')  // .. { subseq }
        return parse_identifier(s);
      else if (isdigit(s->lookahead))  // . { digit }
        return parse_number(s);
      break;
    case '"':
      return parse_string(s);  // " { char | esc } "
    case '\'':
      next(s);
      return cons(s, sy_intern(s, "quote"), cons(s, parse_expr(s), NULL));
    case '`':
      next(s);
      return cons(s, sy_intern(s, "quasiquote"), cons(s, parse_expr(s), NULL));
    case ',':
      next(s);
      if (s->lookahead == '@') {
        next(s);
        return cons(s, sy_intern(s, "unquote-splicing"), cons(s, parse_expr(s), NULL));
      }
      else return cons(s, sy_intern(s, "unquote"), cons(s, parse_expr(s), NULL));
    case '#':
      next(s);
      switch (s->lookahead) {
        case 't':
          next(s);
          return sy_intern(s, "#t");
        case 'f':
          next(s);
          return sy_intern(s, "#f");
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
      s->err = E_EOF;
      return NULL;
    default:
      break;
  }
  return parse_invalid(s);
}

static const char *err_msgs[] = {
  [E_OK] = "",
  [E_EOF] = "unexpected EOF",
  [E_TOKEN] = "invalid token",
  [E_DOTSEP] = "invalid expression following dot separator",
  [E_RANGE] = "number is out of range",
};

static void read_error(SchemeY *s) {
  printf("Read Error: %s\n", err_msgs[s->err]);
  printf("Line %zu, at \"%s\"\n", s->lineno, s->token);
}

// set_input_port - update lineno, lookahead, etc.

// Read the next expression.
cell *sy_read(SchemeY *s, cell *port) {
  s->tp = s->token;
  s->lookahead = s->prompt ? '\n' : EOF;
  s->lineno = 1;
  s->depth = 0;
  s->err = E_OK;

  next(s);
  cell *expr = parse_expr(s);

  if (s->lookahead == EOF)
    return sy_intern(s, "#!eof");

  if (s->err) {
    read_error(s);
    return NULL;
  }
  return expr;
}

static void write_obj(cell *obj, FILE *stream);

static void write_list(cell *obj, FILE *stream) {
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

static void write_vector(cell *obj, FILE *stream) {
  cell *items = as(obj).vector->items;
  size_t len = as(obj).vector->len;
  for (size_t i = 0; i < len - 1; i++) {
    write_obj(items + i, stream);
    fputc(' ', stream);
  }
  write_obj(items + len - 1, stream);
}

// TODO: handle quote, quasiquote, unquote, unquote-splicing, vector, etc.
static void write_obj(cell *obj, FILE *stream) {
  if (!obj)
    fprintf(stream, "()");
  else if (islist(obj)) {
    fputc('(', stream);
    write_list(obj, stream);
    fputc(')', stream);
  }
  else switch (type(obj)) {
    case T_INT:
      fprintf(stream, "%ld", as(obj).integer);
      break;
    case T_REAL:
      if (as(obj).real < 1e-3 || as(obj).real > 1e6)
        fprintf(stream, "%.3e", as(obj).real);
      else
        fprintf(stream, "%.3f", as(obj).real);
      break;
    // case RATIONAL:
    // case COMPLEX:
    // case BOOLEAN:
    //   fprintf(stream, getv(obj).integer ? "#t" : "#f");
    //   break;
    case T_CHAR:
      fprintf(stream, "%c", as(obj).character);
      break;
    case T_STRING:
      fprintf(stream, "\"%s\"", as(obj).string);
      break;
    case T_SYMBOL:
      fprintf(stream, "%s", as(obj).string);
      break;
    case T_FFUN:
      fprintf(stream, "<ffun>");
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

// Print an object to stdout.
void print_obj(cell *obj) {
  write_obj(obj, stdout);
  fputc('\n', stdout);
}

// Write an object to given/default output port.
cell *sy_write(SchemeY *s, cell *args) {
  cell *port = cdr(args) ? car(cdr(args)) : s->outport;
  FILE *stream = as(port).port;
  write_obj(car(args), stream);
  fputc('\n', stream);
  return NULL;
}
