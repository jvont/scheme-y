#include "io.h"
#include "heap.h"
#include "state.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int isdelim(int c) { return isspace(c) || c == '(' || c == ')' || c == ';'; }
static int isinitial(int c) { return isalpha(c) || strchr("!$%&*/:<=>?@^_~", c); }
static int issubseq(int c) { return isalnum(c) || strchr("!$%&*/:<=>?@^_~+-.", c); }

#define save_until(p,pred) while (!pred(p->lookahead)) { save_next(p); } *(p->tp) = '\0'
#define skip_while(p,pred) while (pred(p->lookahead)) next(p)
#define save_next(p) (save(p, p->lookahead), next(p))

enum {
  E_OK,
  E_EOF,
  E_TOKEN,
  E_DOTSEP,
  E_RANGE,
};

typedef struct Parser {
  FILE *port;  /* Current input port. */

  char *token, *tp, *tend;  /* Token buffer */
  int lookahead;  /* Lookahead character */
  size_t depth, lineno;  /* Reader state */
  int prompt;  /* Interactive prompt mode? */

  int error;
} Parser;

/* Get user input, printing prompt to stdout. */
static void input_prompt(Parser *p) {
  if (p->depth) {  // print prompt based on nesting
    for (size_t i = 0; i <= p->depth; i++)
      putc('.', stdout);
  }
  else putc('>', stdout);
  putc(' ', stdout);
  fflush(stdout);
  p->lookahead = fgetc(p->port);
  if (p->lookahead == '\n')  // prompt until non-empty
    input_prompt(p);
}

/* Get the next character from file/interactive prompt. */
static void next(Parser *p) {
  if (p->prompt) {  // prompt
    if (p->lookahead == '\n')
      input_prompt(p);
    else
      p->lookahead = fgetc(stdin);
  }
  else {  // file
    p->lookahead = fgetc(p->port);
    p->lineno += (p->lookahead == '\n');
  }
}

// Object *sy_read_char(State *s, Object *port) {

// }

/* Save character to token buffer, resize buffer if needed. */
static void save(Parser *p, int c) {
  if (p->tp >= p->tend) {  // resize
    if ((uintptr_t)(p->tend - p->token) >= SIZE_MAX / 2) {
      fprintf(stderr, "maximum buffer size reached\n");
      exit(1);
    }
    size_t newsize = 2 * (p->tend - p->token);
    char *newtbase = realloc(p->token, newsize);
    if (!newtbase) {
      fprintf(stderr, "buffer realloc failed\n");
      exit(1);
    }
    p->tp = newtbase + (p->tp - p->token);
    p->token = newtbase;
    p->tend = p->token + newsize;
  }
  *p->tp++ = c;
}

/* Scan an invalid token until next delimiter. */
static Object *parse_invalid(Parser *p) {
  p->error = E_TOKEN;
  save_until(p, isdelim);
  return NULL;
}

/* Number = integer | real ; */
static Object *parse_number(Parser *p) {
  save_until(p, isdelim);
  errno = 0;  /* reset errno before call */
  char *nptr = p->token;
  char *endptr;
  long i = strtol(nptr, &endptr, 0);
  if (errno == ERANGE || *endptr != '\0') {
    float r = strtof(nptr, &endptr);
    if (errno == ERANGE || *endptr != '\0')
      return parse_invalid(p);  /* ERROR: number too large */
    else
      return mk_real(r);
  }
  return mk_integer(i);
}

/* String = " { ( character | escape ) - " } " ; */
static Object *parse_string(Parser *p) {
  next(p);
  while (p->lookahead != '"') {
    if (p->lookahead == '\\')  /* string escape */
      save_next(p);
    else if (p->lookahead == EOF) {  /* unexpected EOF */
      p->error = E_EOF;
      break;
    }
    save_next(p);
  }
  *p->tp = '\0';
  next(p);
  return mk_string(p->token);
}

/* Identifier = initial { subseq } ; */
static Object *parse_identifier(Parser *p) {
  while (issubseq(p->lookahead)) {
    save(p, tolower(p->lookahead));
    next(p);
  }
  *p->tp = '\0';
  // if (isdelim(p->lookahead))
  //   return sy_intern(p, p->token);
  // else
    return parse_invalid(p);
}

static Object *parse_expr(Parser *p);

/* Parse a list expression. */
static Object *parse_list(Parser *p) {
  skip_while(p, isspace);  /* skip whitespace */
  if (p->lookahead == ')') {
    p->depth--;
    next(p);
    return NULL;
  }
  int dotsep = (p->lookahead == '.');

  // Object *obj = parse_expr(p);

  // Object *rest = parse_list(p);

  // if (dotsep && rest)
  //   p->error = E_DOTSEP;
  // return dotsep ? obj : cons(obj, rest);
}

/* Parse an expression. */
static Object *parse_expr(Parser *p) {
  p->tp = p->token;
  skip_while(p, isspace);  /* skip whitespace */
  if (p->lookahead == '(') {  /* list (common) */
    p->depth++;
    next(p);
    // TODO: handle illegal dot separator at start of list
    return parse_list(p);
  }
  else if (isinitial(p->lookahead))  /* identifier */
    return parse_identifier(p);
  else if (isdigit(p->lookahead))  /* number */
    return parse_number(p);
  else switch (p->lookahead) {
    case '+': case '-':
      save_next(p);
      if (p->lookahead == '.' || isdigit(p->lookahead))  /* ( + | - ) [ . ] { digit } */
        return parse_number(p);
      else if (isdelim(p->lookahead))  /* ( + | - ) */
        return parse_identifier(p);
      break;
    case '.':
      save_next(p);
      if (isdelim(p->lookahead))  /* dot separator (if list) */
        return (p->depth) ? parse_expr(p) : parse_invalid(p);
      else if (isinitial(p->lookahead) || p->lookahead == '.')  /* .. { subseq } */
        return parse_identifier(p);
      else if (isdigit(p->lookahead))  /* . { digit } */
        return parse_number(p);
      break;
    case '"':
      return parse_string(p);  /* " { char | esc } " */
    // case '\'':
    //   next(p);
    //   return cons(sy_intern(p->s, "quote"), cons(parse_expr(p), NULL));
    // case '`':
    //   next(p);
    //   return cons(sy_intern(p->s, "quasiquote"), cons(parse_expr(p), NULL));
    // case ',':
    //   next(p);
    //   if (p->lookahead == '@') {
    //     next(p);
    //     return cons(sy_intern(p->s, "unquote-splicing"), cons(parse_expr(p), NULL));
    //   }
    //   else return cons(sy_intern(p->s, "unquote"), cons(parse_expr(p), NULL));
    case '#':
      next(p);
      switch (p->lookahead) {
        // case 't':
        //   next(p);
        //   return sy_intern(p->s, "#t");
        // case 'f':
        //   next(p);
        //   return sy_intern(p->s, "#f");
        case '|':  /* block comment #| ... |# */
        case '\\':  /* character literal (handle special cases) */
        case '(':  /* vector literal */
        case 'e': case 'i': case 'o': case 'd': case 'x':  // number literal
        case 'u':  /* bytevector constant, #u8( */
        default:  /* datum label, #<digits>= #<digits># */
          break;
      }
      break;
    // case EOF:
    //   return sy_intern(p->s, "#!eof");
    default:
      break;
  }
  return parse_invalid(p);
}

static const char *err_msgs[] = {
  [E_OK] = "",
  [E_EOF] = "unexpected EOF",
  [E_TOKEN] = "invalid token",
  [E_DOTSEP] = "invalid expression following dot separator",
  [E_RANGE] = "number is out of range",
};

static void read_error(Parser *p) {
  printf("Read Error: %s\n", err_msgs[p->error]);
  printf("Line %zu, at \"%s\"\n", p->lineno, p->token);
}

// set_input_port - update lineno, lookahead, etc.

/* Read the next expression. */
Object *sy_read(Object *port) {
  Parser p;
  p.token = malloc(1024);
  p.tp = p.token;
  p.lookahead = p.prompt ? '\n' : EOF;
  p.lineno = 1;
  p.depth = 0;
  p.error = E_OK;
  // p.s = s;

  next(&p);
  Object *expr = parse_expr(&p);

  if (p.error) {
    read_error(&p);
    return NULL;
  }
  return expr;
}

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
    // case RATIONAL:
    // case COMPLEX:
    // case BOOLEAN:
    //   fprintf(stream, getv(obj).integer ? "#t" : "#f");
    //   break;
    case T_CHARACTER:
      fprintf(stream, "%c", as(obj).character);
      break;
    case T_STRING:
      fprintf(stream, "\"%s\"", as(obj).string);
      break;
    case T_SYMBOL:
      fprintf(stream, "%s", as(obj).string);
      break;
    case T_PROCEDURE:
      fprintf(stream, "<procedure>");
      break;
    case T_SYNTAX:
      fprintf(stream, "<syntax>");
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
