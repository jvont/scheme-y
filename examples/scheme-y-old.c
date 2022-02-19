/* Scheme-y (C) J. von Tiesenhausen */
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include <readline/readline.h>
// #include <readline/history.h>

typedef struct Object {
  enum {
    ObjInteger, ObjReal, ObjCharacter, ObjString,
    ObjSymbol, ObjBoolean, ObjPair, ObjPrimitive,
    ObjClosure, ObjPort
  } kind;
  union {
    long integer;
    double real;
    int character;
    char *string;
    struct { struct Object *data, *next; } pair;
    struct Object *(*primitive)(struct Object *);
    struct { struct Object *params, *body, *env; } closure;
    struct { FILE *stream; char *mode; } port;
  } as;
} Object;

Object *obj_new(int kind) {
  Object *obj = (Object *)calloc(1, sizeof(Object));
  if (obj == NULL) exit(1);
  obj->kind = kind;
  return obj;
}

char *str_new(char *src) {
  char *dest = (char *)calloc(strlen(src), sizeof(char));
  if (dest == NULL) exit(1);
  return strcpy(dest, src);
}

Object *obj_integer(long integer) {
  Object *obj = obj_new(ObjInteger);
  obj->as.integer = integer;
  return obj;
}

Object *obj_real(double real) {
  Object *obj = obj_new(ObjInteger);
  obj->as.real = real;
  return obj;
}

Object *obj_string(char *str) {
  Object *obj = obj_new(ObjString);
  obj->as.string = str_new(str);
  return obj;
}

Object *obj_symbol(char *str) {
  Object *obj = obj_new(ObjSymbol);
  obj->as.string = str_new(str);
  return obj;
}

Object *obj_pair(Object *car, Object *cdr) {
  Object *obj = obj_new(ObjPair);
  obj->as.pair.data = car;
  obj->as.pair.next = cdr;
  return obj;
}

Object *obj_primitive(Object *(*primitive)(Object *)) {
  Object *obj = obj_new(ObjPrimitive);
  obj->as.primitive = primitive;
  return obj;
}

Object *obj_closure(Object *params, Object *body, Object *env) {
  Object *obj = obj_new(ObjClosure);
  obj->as.closure.params = params;
  obj->as.closure.body = body;
  obj->as.closure.env = env;
  return obj;
}

Object *obj_port(FILE *stream, char *mode) {
  Object *obj = obj_new(ObjPort);
  obj->as.port.stream = stream;
  obj->as.port.mode = mode;
  return obj;
}

// ---------------------------------------------------------------------------
// Builtins
// ---------------------------------------------------------------------------

#define pprint(p,...) fprintf((p)->as.port.stream, __VA_ARGS__)
#define pputc(p,c) fputc((c), (p)->as.port.stream)
#define pgetc(p) fgetc((p)->as.port.stream)

Object *car(Object *obj) { return obj->as.pair.data; }
Object *cdr(Object *obj) { return obj->as.pair.next; }

Object *cons(Object *car, Object *cdr) { return obj_pair(car, cdr); }

void print_obj(Object *port, Object *obj);
void print_list(Object *port, Object *obj) {
  print_obj(port, car(obj));
  if (cdr(obj)) {
    if (cdr(obj)->kind == ObjPair) {
      pputc(port, ' ');
      print_list(port, cdr(obj));
    }
    else {
      pprint(port, " . ");
      print_obj(port, cdr(obj));
    }
  }
}

void print_obj(Object *port, Object *obj) {
  switch (obj->kind) {
    case ObjInteger:
      pprint(port, "%ld", obj->as.integer);
      break;
    case ObjReal:
      pprint(port, "%.3f", obj->as.real);
      break;
    case ObjString:
      pprint(port, "%s", obj->as.string);
      break;
    case ObjSymbol:
      pprint(port, "%s", obj->as.string);
      break;
    case ObjPair:
      fputc('(', port->as.port.stream);
      print_list(port, obj);
      fputc(')', port->as.port.stream);
      break;
    case ObjClosure:
      // fprintf(port->as.port, "", obj->as.closure);
      break;
    default:
      pprint(port, "()");
      break;
  }
}

Object *fcons(Object *arg) { return cons(car(arg), car(cdr(arg))); }
Object *fcar(Object *arg) { return car(car(arg)); }
Object *fcdr(Object *arg) { return cdr(car(arg)); }

// ---------------------------------------------------------------------------
// Scanning
// ---------------------------------------------------------------------------

typedef enum Token {
  TokInvalid, TokEOF, TokDot, TokLParen, TokRParen,
  TokQuote, TokQuasiQuote, TokComma, TokCommaAt, TokVector,
  TokIdentifier, TokBoolean, TokNumber, TokCharacter, TokString
} Token;

typedef struct Scanner {
  Object *port;  // input port
  char *buf;  // token buffer
  size_t p, size;  // buffer position/size
  int ch;  // lookahead character
  Token tok;  // current token
} Scanner;

int isdelim(int c) { return isspace(c) || c == '(' || c == ')' || c == ';'; }
int isinitial(int c) { return isalpha(c) || strchr("!$%&*/:<=>?@^_~", c); }
int issubseq(int c) { return isinitial(c) || isdigit(c) || strchr("+-.", c); }

#define next(s) (s->ch = pgetc(s->port))
#define save_next(s) (save(s, s->ch), next(s))
#define return_next(s,t) return(save_next(s), (t))

void save(Scanner *s, int c) {
  if (s->p >= s->size) {
    s->buf = (char *)realloc(s->buf, 2 * s->size);
    if (s->buf == NULL) exit(1);
  }
  s->buf[s->p++] = c;
  s->buf[s->p] = '\0';
}

int match_next(Scanner *s, int c) {
  if (s->ch == c) {
    save_next(s);
    return 1;
  }
  return 0;
}

int match_next2(Scanner *s, char *pair) {
  assert(pair[2] == '\0');
  if (s->ch == pair[0] || s->ch == pair[1]) {
    save_next(s);
    return 1;
  }
  return 0;
}

Scanner *scanner_new(Object *port) {
  Scanner *s = (Scanner *)malloc(sizeof(Scanner));
  if (s == NULL) exit(1);
  s->buf = (char *)malloc(BUFSIZ * sizeof(char));
  if (s->buf == NULL) exit(1);
  s->port = port;
  s->p = 0;
  s->size = BUFSIZ;
  s->ch = EOF;
  return s;
}

void scanner_free(Scanner *s) {
  if (s != NULL) free(s->buf);
  free(s);
}

// exponent = [ + | - ] { digit }- space ;
int scan_exponent(Scanner *s) {
  match_next2(s, "+-");
  if (!isdigit(s->ch))  // require digit
    return TokInvalid;
  while(isdigit(s->ch))
    save_next(s);
  return isdelim(s->ch) ? TokNumber : TokInvalid;
}

// suffix = { digit } ( exponent | space ) ;
int scan_suffix(Scanner *s) {
  do {
    save_next(s);
    if (match_next2(s, "Ee"))
      return scan_exponent(s);
  } while(isdigit(s->ch));
  return isdelim(s->ch) ? TokNumber : TokInvalid;
}

// number = digits ( suffix | exponent | space ) ;
int scan_number(Scanner *s) {
  int hasdot = (s->ch == '.');
  do {
    save_next(s);
    if (s->ch == '.' && !hasdot)
      return scan_suffix(s);
    else if (match_next2(s, "Ee"))
      return scan_exponent(s);
    // else if (match_next2(s, "+-"))
    //   return scan_complex(s);
    // else if (match_next(s, '/'))
    //   return scan_decimal(s);
  } while(isdigit(s->ch));
  return isdelim(s->ch) ? TokNumber : TokInvalid;
}

// identifier = initial { subseq } space ;
int scan_identifier(Scanner *s) {
  do {
    save_next(s);
  } while(issubseq(s->ch));
  return isdelim(s->ch) ? TokIdentifier : TokInvalid;
}

// scan next token
void scan(Scanner *s) {
  s->p = 0;
  while (isspace(s->ch))  // skip whitespace
    next(s);
  if (isinitial(s->ch))  // identifier
    s->tok = scan_identifier(s);
  else if (isdigit(s->ch))  // number
    s->tok = scan_number(s);
  else switch (s->ch) {
    case EOF:
      s->tok = TokEOF;
      break;
    case '+': case '-':
      save_next(s);
      if (s->ch == '.')  // ( + | - ) . suffix
        s->tok = scan_suffix(s);
      else if (isdigit(s->ch))  // ( + | - ) number
        s->tok = scan_number(s);
      else if (isdelim(s->ch))  // ( + | - )
        s->tok = TokIdentifier;
      else
        s->tok = TokInvalid;
      break;
    case '.':
      save_next(s);
      if (s->ch == '.')  // .. subseq
        s->tok = scan_identifier(s);
      else if (isdigit(s->ch))  // . suffix
        s->tok = scan_suffix(s);
      else if (isdelim(s->ch))  // dotsep
        s->tok = TokDot;
      else
        s->tok = TokInvalid;
      break;
    case '"':
      next(s);
      while (s->ch != '"') {  // string
        if (s->ch == EOF)  // unterminated
          s->tok = TokInvalid;
        else
          save_next(s);
      }
      next(s);
      s->tok = TokString;
      break;
    case '#':
      save_next(s);
      if (match_next2(s, "tf"))
        s->tok = TokBoolean;
      else switch (s->ch) {
        case '(':
          save_next(s);
          s->tok = TokVector;
          break;
        case '\\':
          save_next(s);
          if (isdelim(s->ch)) {
            s->tok = TokCharacter;
            break;
          }
          // FALLTHROUGH
        default:
          s->tok = TokInvalid;
          break;
      }
      break;
    case '(':
      save_next(s);
      s->tok = TokLParen;
      break;
    case ')':
      save_next(s);
      s->tok = TokRParen;
      break;
    case '\'':
      save_next(s);
      s->tok = TokQuote;
      break;
    case '`':
      save_next(s);
      s->tok = TokQuasiQuote;
      break;
    case ',':
      save_next(s);
      if (match_next(s, '@'))
        s->tok = TokCommaAt;
      else
        s->tok = TokComma;
      break;
    default:
      save_next(s);
      s->tok = TokInvalid;
      break;
  }
  s->buf[s->p] = '\0';
}

// ---------------------------------------------------------------------------
// Parsing
// ---------------------------------------------------------------------------

// store string in symbols list 'elegantly'
Object* intern(Object **symbols, char *sym) {
  while (*symbols) {
    int cmp = strcmp(sym, car(*symbols)->as.string);
    if (cmp == 0)
      return car(*symbols);
    else if (cmp > 0)
      symbols = &(*symbols)->as.pair.next;
    else break;
  }
  *symbols = cons(obj_symbol(sym), *symbols);
  return car(*symbols);
}

typedef struct Env {
  Object *symbols;
  Scanner *s;
} Env;

Object *parse_list(Env *);
Object *parse_obj(Env *);

Object *parse_expr(Env *e) {
  scan(e->s);
  return parse_obj(e);
}

Object *parse_obj(Env *e) {
  switch(e->s->tok) {
    case TokInvalid: case TokEOF: return NULL;
    // case TokDot:
    case TokLParen:
      return parse_list(e);
    case TokQuote:
      return cons(
        intern(&e->symbols, "quote"),
        cons(parse_expr(e), NULL)
      );
    case TokQuasiQuote:
      return cons(
        intern(&e->symbols, "quasiquote"),
        cons(parse_expr(e), NULL)
      );
    case TokComma:
      return cons(
        intern(&e->symbols, "unquote"),
        cons(parse_expr(e), NULL)
      );
    case TokCommaAt:
      return cons(
        intern(&e->symbols, "unquote-splicing"),
        cons(parse_expr(e), NULL)
      );
    // case TokVector:
    // case TokIdentifier:
    // case TokBoolean:
    // case TokNumber:
    // case TokCharacter:
    case TokString:
      return obj_string(e->s->buf);
    default:
      return intern(&e->symbols, e->s->buf);
  }
}

Object *parse_list(Env *e) {
  scan(e->s);
  if (e->s->tok == TokRParen)
    return NULL;
  Object *obj = parse_obj(e);
  return cons(obj, parse_list(e));
}

// ---------------------------------------------------------------------------
// Evaluation
// ---------------------------------------------------------------------------

// void eval() {

// }

int main() {
  FILE *stream = fopen("./examples/test_prog.scm", "r");
  Object *input_port = obj_port(stream, "r");

  // Object *input_port = obj_port(stdin, ModeR);
  Object *output_port = obj_port(stdout, "w");
  
  Env *e = (Env *)calloc(1, sizeof(Env));
  Scanner *s = scanner_new(input_port);
  e->s = s;
  next(s);

  Object *prog = parse_expr(e);
  print_obj(output_port, e->symbols);
  puts("");
  print_obj(output_port, prog);
  puts("");

  fclose(stream);

  return 0;
}
