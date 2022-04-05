#include "parser.h"
#include "runtime.h"
#include "heap.h"

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static int isdelim(int c) { return isspace(c) || c == '(' || c == ')' || c == ';'; }
static int isinitial(int c) { return isalpha(c) || strchr("!$%&*/:<=>?@^_~", c); }
static int issubseq(int c) { return isalnum(c) || strchr("!$%&*/:<=>?@^_~+-.", c); }

#define save_until(p,pred) while (!pred(p->lookahead)) { save_next(p); } save(p, '\0')
#define skip_while(p,pred) while (pred(p->lookahead)) next(p)
#define save_next(p) (save(p, p->lookahead), next(p))

typedef struct Parser {
  FILE *port;  // current input port
  char *buffer;  // token buffer
  size_t buffpos, buffsize;
  int lookahead;  // next character
  size_t depth, lineno;  // read position
  Object *exprtail;  // list expression tail
  int prompt;  // interactive prompt?
  SyState *s;  // associated state
} Parser;

static void Parser_init(SyState *s, Parser *p) {
  p->port = DEFAULT_INPUT_PORT;  // TODO: check top of stack for port

  Sy_pushnstring(s, "", BUFFER_SIZE);
  p->buffer = as(&s->stack[s->top - 1]).string;
  // p->buffer = as(Sy_gettop(s)).string;

  p->buffpos = 0;
  p->buffsize = BUFFER_SIZE;

  p->lookahead = p->port == stdin ? '\n' : EOF;
  p->depth = 0;
  p->lineno = 1;
  p->exprtail = NULL;

  p->prompt = (p->port == stdin);
  p->s = s;
}

// Get user input, printing prompt to stdout.
static int input_prompt(Parser *p) {
  int c = '\n';
  while (c == '\n') {
    if (p->depth) {  // print prompt based on nesting
      for (size_t i = 0; i <= p->depth; i++)
        putc('.', stdout);
    }
    else putc('>', stdout);
    putc(' ', stdout);
    fflush(stdout);
    c = fgetc(p->port);
  }
  return c;
}

// Get the next character from file/interactive prompt.
static void next(Parser *p) {
  if (p->prompt) {  // prompt
    if (p->lookahead == '\n')
      p->lookahead = input_prompt(p);
    else
      p->lookahead = fgetc(p->port);
  }
  else {  // file
    p->lookahead = fgetc(p->port);
    p->lineno += (p->lookahead == '\n');
  }
}

// Save character to token buffer, resize buffer if needed.
static void save(Parser *p, int c) {
  if (p->buffpos >= p->buffsize) {    
    p->buffsize *= 2;
    p->buffer = Heap_strndup(p->s->h, p->buffer, p->buffsize);
  }
  p->buffer[p->buffpos++] = c;
}

// Scan an invalid token until next delimiter.
static int parse_invalid(Parser *p) {
  save_until(p, isdelim);
  return E_TOKEN;
}

// Number = integer | real ;
static int parse_number(Parser *p) {
  save_until(p, isdelim);
  errno = 0;  // reset errno before call
  char *nptr = p->buffer;
  char *endptr;
  long i = strtol(nptr, &endptr, 0);
  if (errno == ERANGE || *endptr != '\0') {
    float r = strtof(nptr, &endptr);
    if (errno == ERANGE || *endptr != '\0') {  // ERROR: number too large
      parse_invalid(p);
      return E_PARSE;
    }
    else Sy_pushreal(p->s, r);
  }
  else Sy_pushinteger(p->s, i);
  return E_OK;
}

// String = " { ( character | escape ) - " } " ;
static int parse_string(Parser *p) {
  next(p);
  while (p->lookahead != '"') {
    if (p->lookahead == '\\')  // string escape
      save_next(p);
    else if (p->lookahead == EOF)  // unexpected EOF
      return E_EOF;
    save_next(p);
  }
  save(p, '\0');
  next(p);
  Sy_pushstring(p->s, p->buffer);
  return E_OK;
}

// Identifier = initial { subseq } ;
static int parse_identifier(Parser *p) {
  while (issubseq(p->lookahead)) {
    save(p, tolower(p->lookahead));
    next(p);
  }
  save(p, '\0');
  if (!isdelim(p->lookahead))
    return E_TOKEN;

  Sy_pushstring(p->s, p->buffer);
  // Sy_intern(p->s, p->buffer);
  return E_OK;
}

static int parse_expr(Parser *p);

// Parse a list expression.
static int parse_list(Parser *p) {
  skip_while(p, isspace);  // skip whitespace
  if (p->lookahead == ')') {
    p->depth--;
    next(p);
    return E_OK;
  }
  // int dotsep = (p->lookahead == '.');

  while (p->lookahead != ')') {

    // TODO: API methods for creating lists (cons)

    Sy_pushlist(p->s, NULL, NULL);
    
    Object *cons = &p->s->stack[p->s->top - 1];

    int err = parse_expr(p);

    // Object *val = Sy_pop()

  }

  parse_list(p);

  // if (dotsep && rest)
  //   p->error = E_DOTSEP;
  // return dotsep ? obj : cons(obj, rest);
  return E_OK;
}

// Parse an expression.
static int parse_expr(Parser *p) {
  p->buffpos = 0;
  skip_while(p, isspace);  // skip whitespace
  if (p->lookahead == '(') {  // list (common)
    p->depth++;
    next(p);
    // TODO: handle illegal dot separator at start of list
    return parse_list(p);
  }
  else if (isinitial(p->lookahead))  // identifier
    return parse_identifier(p);
  else if (isdigit(p->lookahead))  // number
    return parse_number(p);
  else switch (p->lookahead) {
    case '+': case '-':
      save_next(p);
      if (p->lookahead == '.' || isdigit(p->lookahead))  // ( + | - ) [ . ] { digit }
        return parse_number(p);
      else if (isdelim(p->lookahead))  // ( + | - )
        return parse_identifier(p);
      break;
    case '.':
      save_next(p);
      if (isdelim(p->lookahead))  // dot separator (if list)
        return (p->depth) ? parse_expr(p) : parse_invalid(p);
      else if (isinitial(p->lookahead) || p->lookahead == '.')  // .. { subseq }
        return parse_identifier(p);
      else if (isdigit(p->lookahead))  // . { digit }
        return parse_number(p);
      break;
    case '"':
      return parse_string(p);  // " { char | esc } "
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
    //   return sy_intern(p->s, "#!eof");
    default:
      break;
  }
  return parse_invalid(p);
}

// static const char *err_msgs[] = {
//   [E_OK] = "",
//   [E_EOF] = "unexpected EOF",
//   [E_TOKEN] = "invalid token",
//   [E_DOTSEP] = "invalid expression following dot separator",
//   [E_RANGE] = "number is out of range",
// };

// static void read_error(Parser *p) {
//   printf("Read Error: %s\n", err_msgs[p->error]);
//   printf("Line %zu, at \"%s\"\n", p->lineno, p->buffer);
// }

// Read the next expression.
int Parser_parse(SyState *s) {
  Parser p;
  Parser_init(s, &p);

  next(&p);
  int r = parse_expr(&p);

  // if (s->err)
  //   read_error(&p);
}
