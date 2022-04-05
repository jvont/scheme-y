#include "runtime.h"
#include "utils.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------------------------
// Scanning utilities
// ---------------------------------------------------------------------------

static int isdelim(int c) { return isspace(c) || c == '(' || c == ')' || c == ';'; }
static int isinitial(int c) { return isalpha(c) || strchr("!$%&*/:<=>?@^_~", c); }
static int issubseq(int c) { return isalnum(c) || strchr("!$%&*/:<=>?@^_~+-.", c); }

#define save_until(r,pred) while (!pred(r->ch)) { save_next(r); } save(r, '\0')
#define skip_while(r,pred) while (pred(r->ch)) next(r)
#define save_next(r) (save(r, r->ch), next(r))

// Get user input, printing prompt to stdout.
static int input_prompt(Reader *r) {
  int c = '\n';
  while (c == '\n') {
    if (r->depth) {  // print prompt based on nesting
      for (size_t i = 0; i <= r->depth; i++)
        putc('.', stdout);
    }
    else putc('>', stdout);
    putc(' ', stdout);
    fflush(stdout);
    c = fgetc(r->port);
  }
  return c;
}

// Get the next character from file/interactive prompt.
static void next(Reader *r) {
  if (r->prompt) {  // prompt
    if (r->ch == '\n')
      r->ch = input_prompt(r);
    else
      r->ch = fgetc(r->port);
  }
  else {  // file
    r->ch = fgetc(r->port);
    r->lineno += (r->ch == '\n');
  }
}

// Save character to token buffer, resize buffer if needed.
static void save(Reader *r, int c) {
  if (r->pos >= r->size) {    
    r->size *= 2;
    r->buffer = err_realloc(r->buffer, r->size);
  }
  r->buffer[r->pos++] = c;
}

// ---------------------------------------------------------------------------
// Reader functions
// ---------------------------------------------------------------------------

// Scan an invalid token until next delimiter.
static int read_invalid(Reader *r) {
  save_until(r, isdelim);
  return E_TOKEN;
}

// Number = integer | real ;
static int read_number(Reader *r) {
  save_until(r, isdelim);
  errno = 0;  // reset errno before call
  char *nptr = r->buffer;
  char *endptr;
  long integer = strtol(nptr, &endptr, 0);
  if (errno == ERANGE || *endptr != '\0') {
    float real = strtof(nptr, &endptr);
    if (errno == ERANGE || *endptr != '\0') {  // ERROR: number too large
      read_invalid(r);
      return E_PARSE;
    }
    else Sy_pushreal(r->s, real);
  }
  else Sy_pushinteger(r->s, integer);
  return E_OK;
}

// String = " { ( character | escape ) - " } " ;
static int read_string(Reader *r) {
  next(r);
  while (r->ch != '"') {
    if (r->ch == '\\')  // string escape
      save_next(r);
    else if (r->ch == EOF)  // unexpected EOF
      return E_EOF;
    save_next(r);
  }
  save(r, '\0');
  next(r);
  Sy_pushstring(r->s, r->buffer);
  return E_OK;
}

// Identifier = initial { subseq } ;
static int read_identifier(Reader *r) {
  while (issubseq(r->ch)) {
    save(r, tolower(r->ch));
    next(r);
  }
  save(r, '\0');
  if (!isdelim(r->ch))
    return E_TOKEN;

  Sy_pushstring(r->s, r->buffer);
  // Sy_intern(r->s, r->buffer);
  return E_OK;
}

static int read_expr(Reader *r);

// Parse a list expression.
static int read_list(Reader *r) {
  skip_while(r, isspace);  // skip whitespace
  if (r->ch == ')') {
    r->depth--;
    next(r);
    return E_OK;
  }
  // int dotsep = (r->ch == '.');

  while (r->ch != ')') {

    // TODO: API methods for creating lists (cons)

    Sy_pushlist(r->s, NULL, NULL);
    
    Object *cons = &r->s->stack[r->s->top - 1];

    int err = read_expr(r);

    // Object *val = Sy_pop()

  }

  read_list(r);

  // if (dotsep && rest)
  //   r->error = E_DOTSEP;
  // return dotsep ? obj : cons(obj, rest);
  return E_OK;
}

// Parse an expression.
static int read_expr(Reader *r) {
  r->pos = 0;
  skip_while(r, isspace);  // skip whitespace
  if (r->ch == '(') {  // list (common)
    r->depth++;
    next(r);
    // TODO: handle illegal dot separator at start of list
    return read_list(r);
  }
  else if (isinitial(r->ch))  // identifier
    return read_identifier(r);
  else if (isdigit(r->ch))  // number
    return read_number(r);
  else switch (r->ch) {
    case '+': case '-':
      save_next(r);
      if (r->ch == '.' || isdigit(r->ch))  // ( + | - ) [ . ] { digit }
        return read_number(r);
      else if (isdelim(r->ch))  // ( + | - )
        return read_identifier(r);
      break;
    case '.':
      save_next(r);
      if (isdelim(r->ch))  // dot separator (if list)
        return (r->depth) ? read_expr(r) : read_invalid(r);
      else if (isinitial(r->ch) || r->ch == '.')  // .. { subseq }
        return read_identifier(r);
      else if (isdigit(r->ch))  // . { digit }
        return read_number(r);
      break;
    case '"':
      return read_string(r);  // " { char | esc } "
    // case '\'':
    //   next(r);
    //   return cons(sy_intern(r->s, "quote"), cons(read_expr(r), NULL));
    // case '`':
    //   next(r);
    //   return cons(sy_intern(r->s, "quasiquote"), cons(read_expr(r), NULL));
    // case ',':
    //   next(r);
    //   if (r->ch == '@') {
    //     next(r);
    //     return cons(sy_intern(r->s, "unquote-splicing"), cons(read_expr(r), NULL));
    //   }
    //   else return cons(sy_intern(r->s, "unquote"), cons(read_expr(r), NULL));
    case '#':
      next(r);
      switch (r->ch) {
        // case 't':
        //   next(r);
        //   return sy_intern(r->s, "#t");
        // case 'f':
        //   next(r);
        //   return sy_intern(r->s, "#f");
        case '|':  // block comment #| ... |#
        case '\\':  // character literal (handle special cases)
        case '(':  // vector literal
        case 'e': case 'i': case 'o': case 'd': case 'x':  // number literal
        case 'u':  // bytevector constant, #u8(
        default:  // datum label, #<digits>= #<digits>#
          break;
      }
      break;
    // case EOF:
    //   return sy_intern(r->s, "#!eof");
    default:
      break;
  }
  return read_invalid(r);
}

// ---------------------------------------------------------------------------
// Read builtin
// ---------------------------------------------------------------------------

int read_read(SyState *s) {
  Reader *r = &s->r;
  r->port = DEFAULT_INPUT_PORT;  // TODO: check top of stack for port
  if (!r->buffer) {
    r->buffer = malloc(BUFFER_SIZE);
    r->size = BUFFER_SIZE;
  }
  r->pos = 0;
  r->ch = r->port == stdin ? '\n' : EOF;
  r->depth = 0;
  r->lineno = 1;
  r->prompt = (r->port == stdin);
  r->s = s;

  next(r);
  return read_expr(r);
}

// TODO: register read function
