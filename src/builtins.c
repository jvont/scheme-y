#include "builtins.h"
#include "scanner.h"

#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// #define pprint(p,...) fprintf(p->as.port.stream, __VA_ARGS__)
// #define pputc(p,c) fputc(c, p->as.port.stream)
// #define pgetc(p) fgetc(p->as.port.stream)

Object *car(Object *obj) { return obj->as.cell.car; }
Object *cdr(Object *obj) { return obj->as.cell.cdr; }
Object *cons(Object *car, Object *cdr) { return obj_cell(car, cdr); }

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
    case ObjBoolean:
      printf(expr->as.integer ? "#t" : "#f");
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
// static Object* intern(Object **symbols, char *sym) {
//   while (*symbols) {
//     int cmp = strcmp(sym, car(*symbols)->as.string);
//     if (cmp == 0)
//       return car(*symbols);
//     else if (cmp > 0)
//       symbols = &(*symbols)->as.cell.cdr;
//     else break;
//   }
//   *symbols = cons(obj_symbol(sym), *symbols);
//   return car(*symbols);
// }

// ---------------------------------------------------------------------------
// Builtins
// ---------------------------------------------------------------------------

// TODO: arity checks

Object *fadd(Object *args) {
  Object *r = obj_integer(0);
  for (; args; args = cdr(args)) {
    Object *a = car(args);
    if (a->kind == ObjInteger) {
      if (r->kind == ObjInteger)
        r->as.integer += a->as.integer;
      else
        r->as.real += (double)a->as.integer;
    }
    else if (a->kind == ObjReal) {
      if (r->kind == ObjInteger) {
        r->kind = ObjReal;
        r->as.real = (double)r->as.integer + a->as.real;
      }
      else r->as.real += a->as.real;
    }
    else return NULL;
  }
  return r;
}

Object *fsub(Object *args) {
  Object *r = obj_integer(0);
  for (; args; args = cdr(args)) {
    Object *a = car(args);
    if (a->kind == ObjInteger) {
      if (r->kind == ObjInteger)
        r->as.integer -= a->as.integer;
      else
        r->as.real -= (double)a->as.integer;
    }
    else if (a->kind == ObjReal) {
      if (r->kind == ObjInteger) {
        r->kind = ObjReal;
        r->as.real = (double)r->as.integer - a->as.real;
      }
      else r->as.real -= a->as.real;
    }
    else return NULL;
  }
  return r;
}

Object *fmul(Object *args) {
  Object *r, *a = car(args);
  if (a->kind == ObjInteger)
    r = obj_integer(a->as.integer);
  else if (a->kind == ObjReal)
    r = obj_real(a->as.real);
  else
    return NULL;
  for (args = cdr(args); args; args = cdr(args)) {
    a = car(args);
    if (a->kind == ObjInteger) {
      if (r->kind == ObjInteger)
        r->as.integer *= a->as.integer;
      else
        r->as.real *= (double)a->as.integer;
    }
    else if (a->kind == ObjReal) {
      if (r->kind == ObjInteger) {
        r->kind = ObjReal;
        r->as.real = (double)r->as.integer * a->as.real;
      }
      else r->as.real *= a->as.real;
    }
    else return NULL;
  }
  return r;
}

Object *fdiv(Object *args) {
  Object *r, *a = car(args);
  if (a->kind == ObjInteger)
    r = obj_integer(a->as.integer);
  else if (a->kind == ObjReal)
    r = obj_real(a->as.real);
  else
    return NULL;
  for (args = cdr(args); args; args = cdr(args)) {
    a = car(args);
    if (a->kind == ObjInteger) {
      if (r->kind == ObjInteger)
        r->as.integer /= a->as.integer;
      else
        r->as.real /= (double)a->as.integer;
    }
    else if (a->kind == ObjReal) {
      if (r->kind == ObjInteger) {
        r->kind = ObjReal;
        r->as.real = (double)r->as.integer / a->as.real;
      }
      else r->as.real /= a->as.real;
    }
    else return NULL;
  }
  return r;
}

// ---------------------------------------------------------------------------
// Parsing
// ---------------------------------------------------------------------------

// Object *parse_number(Env *e) {
//   char *nptr = e->s->buf;
//   char *endptr;
//   long i = strtol(nptr, &endptr, 0);
//   if (errno == ERANGE)
//     return NULL; // ERROR: number too large
//   if (*endptr != '\0' || nptr == endptr) {
//     double d = strtod(nptr, &endptr);
//     if (errno == ERANGE)
//       return NULL;  // ERROR: number too large
//     return obj_real(d);
//   }
//   return obj_integer(i);
// }

// static Object *parse_expr(Env *e);
// Object *parse(Env *e);

// static Object *parse_list(Env *e) {
//   scan(e->s);
//   if (e->s->tok == TokRParen)
//     return NULL;
//   Object *obj = parse_expr(e);
//   return cons(obj, parse_list(e));
// }

// static Object *parse_expr(Env *e) {
//   switch(e->s->tok) {
//     case TokInvalid: case TokEOF:
//       return NULL;
//     // case TokDot:
//     case TokLParen:
//       return parse_list(e);
//     case TokQuote:
//       return cons(
//         intern(&e->symbols, "quote"),
//         cons(parse(e), NULL)
//       );
//     case TokQuasiQuote:
//       return cons(
//         intern(&e->symbols, "quasiquote"),
//         cons(parse(e), NULL)
//       );
//     case TokComma:
//       return cons(
//         intern(&e->symbols, "unquote"),
//         cons(parse(e), NULL)
//       );
//     case TokCommaAt:
//       return cons(
//         intern(&e->symbols, "unquote-splicing"),
//         cons(parse(e), NULL)
//       );
//     // case TokVector:
//     case TokIdentifier:
//       return intern(&e->symbols, e->s->buf);
//     // case TokBoolean:
//     case TokNumber:
//       return parse_number(e);
//     // case TokCharacter:
//     case TokString:
//       return obj_string(e->s->buf);
//     default:
//       return NULL;  // ERROR: Unhandled token
//   }
// }

// // Parse the next expression.
// Object *parse(Env *e) {
//   scan(e->s);
//   return parse_expr(e);
// }

// Evaluate an expression.
Object *eval(Object *expr, Env *e) {
  // if symbol, lookup

  switch (expr->kind) {
    case ObjPair: {
      char *op = car(expr)->as.string;
      Object *args = cdr(expr);
      if (!strcmp(op, "+"))
        return fadd(args);
      else if (!strcmp(op, "-"))
        return fsub(args);
      else if (!strcmp(op, "*"))
        return fmul(args);
      else if (!strcmp(op, "/"))
        return fdiv(args);
      else
        return NULL;
    }
    case ObjSymbol:
    default:
      return expr;
  }
  
  return expr;
}
