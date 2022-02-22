#include "scheme-y.h"

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

static SchemeY *sy_new() {
  SchemeY *s = (SchemeY *)malloc(sizeof(SchemeY));
  if (s == NULL) exit(1);
  s->stream = NULL;
  s->line = s->i = NULL;
  s->ch = '\n';  // prompt flag
  s->buf = (char *)malloc(BUFSIZ * sizeof(char));
  if (s->buf == NULL) exit(1);
  s->p = 0;
  s->size = BUFSIZ;
  s->lineno = 1;
  s->prompt = false;
  s->depth = 0;
  s->err = ErrOK;
  return s;
}

// Create a new scanner from a stream.
SchemeY *sy_stream(FILE *stream) {
  SchemeY *s = sy_new();
  s->stream = stream;
  return s;
}

// Create a new scanner for interactive input.
SchemeY *sy_prompt() {
  SchemeY *s = sy_new();
  s->stream = stdin;
  s->prompt = true;
  return s;
}

// Free scanner + token buffer.
void sy_free(SchemeY *s) {
  if (s != NULL) {
    free(s->line);
    free(s->buf);
  }
  free(s);
}

// ---------------------------------------------------------------------------
// Scanning
// ---------------------------------------------------------------------------

static int isdelim(int c) { return isspace(c) || c == '(' || c == ')' || c == ';'; }
static int isinitial(int c) { return isalpha(c) || strchr("!$%&*/:<=>?@^_~", c); }
static int issubseq(int c) { return isinitial(c) || isdigit(c) || strchr("+-.", c); }

// Get user input, printing prompt to stdout.
static void input_prompt(SchemeY *s) {
  for (unsigned int i = 0; i <= s->depth; i++)
    putc('>', stdout);
  putc(' ', stdout);
#ifdef USE_READLINE
  free(s->line);
  s->line = s->i = readline("");
  add_history(s->line);
  s->ch = *s->i++;
#else
  fflush(stdout);
  s->ch = fgetc(s->stream);
#endif
}

// Get the next character from file stream/interactive prompt.
static void next(SchemeY *s) {
  if (s->prompt) {
#ifdef USE_READLINE
    if (s->ch == '\n')
      input_prompt(s);
    else if (s->line && *s->i)
      s->ch = *s->i++;
    else
      s->ch = '\n';
#else
    if (s->ch == '\n')
      input_prompt(s);
    else
      s->ch = fgetc(s->stream);
#endif
  }
  else {
    s->ch = fgetc(s->stream);
    s->lineno += (s->ch == '\n');
  }
}

// Save character to token buffer, resize buffer if needed.
static void save(SchemeY *s, int c) {
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

static void save_next(SchemeY *s) {
  save(s, s->ch);
  next(s);
}

static void save_until(SchemeY *s, int (*p)(int)) {
  while (!p(s->ch))
    save_next(s);
  s->buf[s->p] = '\0';
}

static void save_while(SchemeY *s, int (*p)(int)) {
  while (p(s->ch))
    save_next(s);
  s->buf[s->p] = '\0';
}

static void skip_while(SchemeY *s, int (*p)(int)) {
  while (p(s->ch))
    next(s);
}

// ---------------------------------------------------------------------------
// Parsing
// ---------------------------------------------------------------------------

// Scan an invalid token until next delimiter.
static Object *parse_invalid(SchemeY *s) {
  s->err = ErrInvalid;
  save_until(s, isdelim);
  return NULL;
}

// Number = integer | real ;
static Object *parse_number(SchemeY *s) {
  save_until(s, isdelim);
  char *nptr = s->buf;
  char *endptr;
  long i = strtol(nptr, &endptr, 0);
  if (errno == ERANGE || *endptr != '\0') {
    double d = strtod(nptr, &endptr);
    if (errno == ERANGE || *endptr != '\0')
      return parse_invalid(s);  // ERROR: number too large
    else
      return obj_real(d);
  }
  return obj_integer(i);
}

// String = " { ( character | escape ) - " } " ;
static Object *parse_string(SchemeY *s) {
  next(s);
  while (s->ch != '"') {
    if (s->ch == '\\')  // string escape
      save_next(s);
    else if (s->ch == EOF) {  // unexpected EOF
      s->err = ErrEOF;
      break;
    }
    save_next(s);
  }
  next(s);
  return obj_string(s->buf);
}

// Identifier = initial { subseq } ;
static Object *parse_identifier(SchemeY *s) {
  save_while(s, issubseq);
  s->buf[s->p] = '\0';
  return isdelim(s->ch) ? obj_symbol(s->buf) : parse_invalid(s);
}

static Object *parse_expr(SchemeY *s);

// Parse a list expression.
static Object *parse_list(SchemeY *s) {
  skip_while(s, isspace);  // skip whitespace
  if (s->ch == ')') {
    s->depth--;
    next(s);
    return NULL;
  }
  bool dotsep = (s->ch == '.');
  Object *obj = parse_expr(s);
  Object *rest = parse_list(s);
  if (dotsep && rest)
    s->err = ErrDotsep;
  return dotsep ? obj : obj_cell(obj, rest);
}

// Parse an expression.
static Object *parse_expr(SchemeY *s) {
  s->p = 0;
  skip_while(s, isspace);  // skip whitespace
  if (s->ch == '(') {  // list (common)
    s->depth++;
    next(s);
    return parse_list(s);
  }
  else if (isinitial(s->ch))  // identifier
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
      if (isdelim(s->ch))  // dot separator (if list)
        return (s->depth) ? parse_expr(s) : parse_invalid(s);
      else if (isinitial(s->ch) || s->ch == '.')  // .. { subseq }
        return parse_identifier(s);
      else if (isdigit(s->ch))  // . { digit }
        return parse_number(s);
      break;
    case '"':
      return parse_string(s);
    case '#':
      next(s);
      switch (s->ch) {
        case 't':
          return obj_boolean(true);
        case 'f':
          return obj_boolean(false);
        case '(':  // vector
        case '\\':  // character
        default:
          break;
      }
    case '\'':
      next(s);
      return obj_symbol("quote");
    case '`':
      next(s);
      return obj_symbol("quasiquote");
    case ',':
      next(s);
      if (s->ch == '@')
        return obj_symbol("unquote-splicing");
      else
        return obj_symbol("unquote");
    case EOF:
      s->err = ErrEOF;
      return NULL;
    default:
      break;
  }
  return parse_invalid(s);
}

// Parse a series of expressions.
Object *sy_parse(SchemeY *s) {
  s->err = ErrOK;
  next(s);
  return parse_expr(s);
}

// ---------------------------------------------------------------------------
// Error handling
// ---------------------------------------------------------------------------

static const char *err_msgs[] = {
  [ErrOK] = "no error found",
  [ErrEOF] = "unexpected EOF",
  [ErrInvalid] = "invalid token",
  [ErrDotsep] = "invalid dot separator",
};

// Print the current error status message.
void sy_error(SchemeY *s) {
  assert(s->err >= 0 && s->err < sizeof(err_msgs) / sizeof(*err_msgs));
  printf("Syntax Error (%zu): %s\n", s->lineno, err_msgs[s->err]);
}
