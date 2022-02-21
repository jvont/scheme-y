#include "scanner.h"

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

static int isdelim(int c) { return isspace(c) || c == '(' || c == ')' || c == ';' || c == EOF; }
static int isinitial(int c) { return isalpha(c) || strchr("!$%&*/:<=>?@^_~", c); }
static int issubseq(int c) { return isinitial(c) || isdigit(c) || strchr("+-.", c); }
static int isdoublequote(int c) { return c == '"'; }

// Get the next character from input, else port if NULL.
void next(Scanner *s) {
  if (s->input) {
    if (*s->i == '\0')
  }
    s->ch = (*s->i == '\0') ? EOF : *s->i++;
  else  // port
    s->ch = fgetc(s->stream);
}

static void save(Scanner *s, int c) {
  if (s->p >= s->size) {
    if (s->size > SIZE_MAX / 2) {
      fprintf(stderr, "maximum token buffer size reached\n");
      exit(1);
    }
    s->buf = (char *)realloc(s->buf, 2 * s->size);
    if (s->buf == NULL) exit(1);
  }
  s->buf[s->p++] = c;
  s->buf[s->p] = '\0';
}

static void save_next(Scanner *s) {
  save(s, s->ch);
  next(s);
}

static void save_until(Scanner *s, int (*p)(int)) {
  while (!p(s->ch))
    save_next(s);
  s->buf[s->p] = '\0';
}

static void save_while(Scanner *s, int (*p)(int)) {
  while (p(s->ch))
    save_next(s);
  s->buf[s->p] = '\0';
}

// Get user input, printing prompt to stdout.
static void input(Scanner *s) {
  // print prompt
  if (s->depth) {
    for (int i = 0; i < s->depth * 2; i++)
      putc('.', stdin);
  }
  else putc('>', stdin);
  putc(' ', stdin);

#ifdef USE_READLINE
  free(s->input);
  s->input = s->i = readline("");
  add_history(s->input);
#else
  fflush(stdout);
#endif
  next(s);
}

// Create a new scanner from a stream.
Scanner *scanner_stream(FILE *stream) {
  Scanner *s = (Scanner *)malloc(sizeof(Scanner));
  if (s == NULL) exit(1);
  s->buf = (char *)malloc(BUFSIZ * sizeof(char));
  if (s->buf == NULL) exit(1);
  s->stream = stream;
  s->input = s->i = NULL;
  s->p = 0;
  s->size = BUFSIZ;
  s->prompt = false;
  s->depth = 0;
  s->ch = EOF;
  return s;
}

// Create a new scanner for interactive input.
Scanner *scanner_prompt() {
  Scanner *s = (Scanner *)malloc(sizeof(Scanner));
  if (s == NULL) exit(1);
  s->buf = (char *)malloc(BUFSIZ * sizeof(char));
  if (s->buf == NULL) exit(1);
  s->stream = stdin;
  s->input = s->i = NULL;
  s->p = 0;
  s->size = BUFSIZ;
  s->prompt = true;
  s->depth = 0;
  s->ch = EOF;
  return s;
}

// Free scanner + token buffer.
void scanner_free(Scanner *s) {
  if (s != NULL)
    free(s->buf);
  free(s);
}

// San until next delimiter.
static Object *parse_error(Scanner *s) {
  save_until(s, isdelim);
  return NULL;
}

// number = integer | real ;
static Object *parse_number(Scanner *s) {
  save_until(s, isdelim);
  char *nptr = s->buf;
  char *endptr;
  long i = strtol(nptr, &endptr, 0);
  if (errno == ERANGE || *endptr != '\0') {
    double d = strtod(nptr, &endptr);
    if (errno == ERANGE || *endptr != '\0')
      return parse_error(s);  // ERROR: number too large
    else
      return obj_real(d);
  }
  return obj_integer(i);
}

// identifier = initial { subseq } ;
static Object *parse_identifier(Scanner *s) {
  save_while(s, issubseq);
  s->buf[s->p] = '\0';
  return isdelim(s->ch) ? obj_symbol(s->buf) : parse_error(s);
}

static Object *parse_expr(Scanner *s);

// Parse a list expression.
static Object *parse_list(Scanner *s) {
  // next(s);
  if (s->ch == ')')
    return NULL;
  Object *obj = parse(s);
  return obj_cell(obj, parse_list(s));
}

// Parse the next expression.
static Object *parse_expr(Scanner *s) {
  s->p = 0;
  while (isspace(s->ch))  // skip whitespace
    next(s);
  if (isinitial(s->ch))  // identifier
    return parse_identifier(s);
  else if (isdigit(s->ch))  // number
    return parse_number(s);
  else switch (s->ch) {
    case '+': case '-':
      save_next(s);
      if (s->ch == '.' || isdigit(s->ch))  // ( + | - ) [ . ] { digit }
        return parse_number(s);
      else if (isdelim(s->ch))  // ( + | - )
        return parse_identifier(s);
      break;
    case '.':
      save_next(s);
      // if (s->ch == '\0')  // dotsep
      //   return
      if (s->ch == '.')  // .. { subseq }
        return parse_identifier(s);
      else if (isdigit(s->ch))  // . { digit }
        return parse_number(s);
      break;
    case '"':
      next(s);
      save_until(s, isdoublequote);
      next(s);
      return obj_string(s->buf);
    case '#':
      next(s);
      switch (s->ch) {
        case 't':
          return obj_boolean(1);
        case 'f':
          return obj_boolean(0);
        case '(':  // vector
        case '\\':  // character
        default:
          break;
      }
    case '(':
      next(s);
      return parse_list(s);
    // case '\'':  // quote
    //   break;
    // case '`':  // quasiquote
    //   break;
    // case ',':
    //   save_next(s);
    //   if (s->ch == '@')  // ,@
    //   else
    //   break;
    // case EOF:
    default:
      break;
  }
  return parse_error(s);
}

// Parse a series of expressions.
Object *parse(Scanner *s) {
  next(s);

  return parse_expr(s);
}
