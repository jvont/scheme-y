#include "reader.h"
#include "state.h"

#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

static int isdelim(int c) { return isspace(c) || c == '(' || c == ')' || c == ';'; }
static int isinitial(int c) { return isalpha(c) || strchr("!$%&*/:<=>?@^_~", c); }
static int issubseq(int c) { return isinitial(c) || isdigit(c) || strchr("+-.", c); }

#define save_until(r,pred) while (!pred(r->ch)) { save_next(r); } r->buf[r->bp] = '\0'
#define skip_while(r,pred) while (pred(r->ch)) next(r)

#define save_next(r) (save(r, r->ch), next(r))

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
static cell *parse_invalid(SyReader *r) {
  r->err = ErrInvalid;
  save_until(r, isdelim);
  return NULL;
}

// Number = integer | real ;
static cell *parse_number(SyReader *r) {
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
static cell *parse_string(SyReader *r) {
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
static cell *parse_identifier(SyReader *r) {
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

static cell *parse_expr(SyReader *r);

// Parse a list expression.
static cell *parse_list(SyReader *r) {
  skip_while(r, isspace);  // skip whitespace
  if (r->ch == ')') {
    r->depth--;
    next(r);
    return NULL;
  }
  bool dotsep = (r->ch == '.');
  cell *obj = parse_expr(r);
  cell *rest = parse_list(r);
  if (dotsep && rest)
    r->err = ErrDotsep;
  return dotsep ? obj : syO_cons(r->s, obj, rest);
}

// Parse an expression.
static cell *parse_expr(SyReader *r) {
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

static void reader_init(SchemeY *s, SyReader *r, cell *port) {
  r->stream = port->as.port.stream;
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
cell *syR_read(SchemeY *s, cell *arg) {
  SyReader r;
  reader_init(s, &r, arg ? car(arg) : s->input_port);
  next(&r);
  cell *expr = parse_expr(&r);
  if (r.ch == EOF)
    return syS_intern(s, "#!eof");
  else if (r.err) {
    read_error(&r);
    return NULL;
  }
  // add expression to heap/stack
  return expr;
}
