#include "builtins.h"

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
