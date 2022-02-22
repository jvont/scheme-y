#include "builtins.h"

#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// #define pprint(p,...) fprintf(p->as.port.stream, __VA_ARGS__)
// #define pputc(p,c) fputc(c, p->as.port.stream)
// #define pgetc(p) fgetc(p->as.port.stream)

Obj *car(Obj *obj) { return obj->as.cell.car; }
Obj *cdr(Obj *obj) { return obj->as.cell.cdr; }
Obj *cons(Obj *car, Obj *cdr) { return obj_cell(car, cdr); }

static void print_list(Obj *expr) {
  print_expr(car(expr));
  if (cdr(expr)) {
    if (cdr(expr)->kind == ObjCell) {
      fputc(' ', stdout);
      print_list(cdr(expr));
    }
    else {
      printf(" . ");
      print_expr(cdr(expr));
    }
  }
}

void print_expr(Obj *expr) {
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
    case ObjCell:
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
// static Obj* intern(Obj **symbols, char *sym) {
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

Obj *fadd(Obj *args) {
  Obj *r = obj_integer(0);
  for (; args; args = cdr(args)) {
    Obj *a = car(args);
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

Obj *fsub(Obj *args) {
  Obj *r = obj_integer(0);
  for (; args; args = cdr(args)) {
    Obj *a = car(args);
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

Obj *fmul(Obj *args) {
  Obj *r, *a = car(args);
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

Obj *fdiv(Obj *args) {
  Obj *r, *a = car(args);
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
Obj *eval(Obj *expr, Env *e) {
  // if symbol, lookup

  switch (expr->kind) {
    case ObjCell: {
      char *op = car(expr)->as.string;
      Obj *args = cdr(expr);
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
