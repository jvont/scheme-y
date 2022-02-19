#include "builtins.h"
#include "scanner.h"

#include <string.h>

// #define pprint(p,...) fprintf(p->as.port.stream, __VA_ARGS__)
// #define pputc(p,c) fputc(c, p->as.port.stream)
// #define pgetc(p) fgetc(p->as.port.stream)

Object *car(Object *obj) { return obj->as.pair.data; }
Object *cdr(Object *obj) { return obj->as.pair.next; }
Object *cons(Object *car, Object *cdr) { return obj_pair(car, cdr); }

static void print_list(Object *expr) {
  print_expr(car(expr));
  if (cdr(expr)) {
    if (cdr(expr)->kind == ObjPair) {
      fputc(' ', stdout);
      print_list(cdr(expr));
    }
    else {
      printf(" . ");
      print_expr(cdr(expr));
    }
  }
}

void print_expr(Object *expr) {
  switch (expr->kind) {
    case ObjInteger:
      printf("%ld", expr->as.integer);
      break;
    case ObjReal:
      printf("%.3f", expr->as.real);
      break;
    case ObjString:
      printf("%s", expr->as.string);
      break;
    case ObjSymbol:
      printf("%s", expr->as.string);
      break;
    case ObjPair:
      fputc('(', stdout);
      print_list(expr);
      fputc(')', stdout);
      break;
    case ObjClosure:
      break;
    default:
      printf("()");
      break;
  }
}

// store string in symbols list 'elegantly'
static Object* intern(Object **symbols, char *sym) {
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

// ---------------------------------------------------------------------------
// Parsing
// ---------------------------------------------------------------------------

static Object *parse_obj(Env *e);
static Object *parse_expr(Env *e);

static Object *parse_list(Env *e) {
  scan(e->s);
  if (e->s->tok == TokRParen)
    return NULL;
  Object *obj = parse_obj(e);
  return cons(obj, parse_list(e));
}

static Object *parse_obj(Env *e) {
  switch(e->s->tok) {
    case TokInvalid: case TokEOF:
      return NULL;
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

static Object *parse_expr(Env *e) {
  scan(e->s);
  return parse_obj(e);
}

// Evaluate the next expression.
Object *eval(Env *e) {
  Object *expr = parse_expr(e);
  print_expr(expr);
  fputc('\n', stdout);
  return expr;
}
