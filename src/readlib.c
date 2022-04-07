#include "runtime.h"
#include "utils.h"

#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

static int isdelim(int c) { return isspace(c) || c == '(' || c == ')' || c == ';' || c == EOF; }
static int isinitial(int c) { return isalpha(c) || strchr("!$%&*/:<=>?@^_~", c); }
static int issubseq(int c) { return isalnum(c) || strchr("!$%&*/:<=>?@^_~+-.", c); }

#define save_until(r,pred) while (!pred(r->ch)) { save_next(r); } save(r, '\0')
#define skip_while(r,pred) while (pred(r->ch)) { next(r); }
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

// Scan an invalid token until next delimiter.
static int read_error(Reader *r) {
  int eeof = (r->ch == EOF);

  // sy_pushstring(r->s, "syntax error");

  save_until(r, isdelim);
  return E_SYNTAX;
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
      return read_error(r);
    }
    else sy_pushreal(r->s, real);
  }
  else sy_pushinteger(r->s, integer);
  return E_OK;
}

// Boolean = #t | #f ;
static int read_boolean(Reader *r) {
  int b = (r->ch == 't');
  next(r);
  if (!isdelim(r->ch))
    return read_error(r);
  
  sy_pushboolean(r->s, b);
  return E_OK;
}

// String = " { ( character | escape ) - " } " ;
static int read_string(Reader *r) {
  next(r);
  while (r->ch != '"') {
    if (r->ch == '\\')  // string escape
      save_next(r);
    else if (r->ch == EOF)  // unexpected EOF
      return read_error(r);
    save_next(r);
  }
  save(r, '\0');
  next(r);
  sy_pushstring(r->s, r->buffer);
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
    return read_error(r);

  sy_pushstring(r->s, r->buffer);
  // sy_pushsymbol(r->s, r->buffer);
  return E_OK;
}

static int read_expr(Reader *r);

#include "heap.h"

int sy_cons(SyState *s) {
  Object *x = SyState_push(s);
  
  // TODO: proper handling of nil

  // Move car to heap
  if (type(&s->stack[s->top - 3]) == T_NIL)
    car(x) = NULL;
  else {
    car(x) = Heap_object(s->h);
    *car(x) = s->stack[s->top - 3];
  }

  // Move cdr to heap
  if (type(&s->stack[s->top - 2]) == T_NIL)
    cdr(x) = NULL;
  else {
    cdr(x) = Heap_object(s->h);
    *cdr(x) = s->stack[s->top - 2];
  }

  // Move 
  s->stack[s->top - 3] = *x;
  s->top -= 2;
}

// Parse a list expression.
static int read_list(Reader *r) {
  skip_while(r, isspace);  // skip whitespace

  if (r->ch == ')') {
    r->depth--;
    next(r);
    sy_pushnil(r->s);
    return E_OK;
  }
  int dotsep = (r->ch == '.');

  int err;
  if ((err = read_expr(r)) || (err = read_list(r)))
    return err;

  if (dotsep) {
    if (type(&r->s->stack[r->s->top - 1]) != T_NIL)
      return read_error(r);

    SyState_pop(r->s);  // remove nil
  }
  else sy_cons(r->s);

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
        return (r->depth) ? read_expr(r) : read_error(r);
      else if (isinitial(r->ch) || r->ch == '.')  // .. { subseq }
        return read_identifier(r);
      else if (isdigit(r->ch))  // . { digit }
        return read_number(r);
      break;
    case '"':
      return read_string(r);  // " { char | esc } "
    // case '\'':
    //   next(r);
    //   sy_intern(r->s, "quote");
    //   read_expr(r);
    // case '`':
    //   next(r);
    //   sy_intern(r->s, "quasiquote");
    //   read_expr(r);
    // case ',':
    //   next(r);
    //   if (r->ch == '@') {
    //     next(r);
    //     sy_intern(r->s, "unquote-splicing");
    //     read_expr(r);
    //   }
    //   sy_intern(r->s, "unquote");
    //   read_expr(r);
    // case '#':
    //   next(r);
    //   switch (r->ch) {
    //     case 't': case 'f':
    //       return read_boolean(r);
    //     case '|':  // block comment #| ... |#
    //     case '\\':  // character literal (handle special cases)
    //     case '(':  // vector literal
    //     case 'e': case 'i': case 'o': case 'd': case 'x':  // number literal
    //     case 'u':  // bytevector constant, #u8(
    //     default:  // datum label, #<digits>= #<digits>#
    //       break;
    //   }
    default:
      break;
  }
  return read_error(r);
}

// TODO: push closurewhen calling

int read_read(SyState *s) {
  Reader *r = &s->r;
  r->port = DEFAULT_INPUT_PORT;  // TODO: check top of stack for port argument
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
  // skip_while(r, isspace);
  // if (r->ch == EOF)
  //   return sy_intern(s, "#!eof");

  return read_expr(r);
}

// TODO: register read function
