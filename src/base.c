#include "base.h"

#include <errno.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// ---------------------------------------------------------------------------
// base
// ---------------------------------------------------------------------------

// TODO: arity checks

cell *base_car(SchemeY *s, cell *arg) { return car(car(arg)); }
cell *base_cdr(SchemeY *s, cell *arg) { return car(car(arg)); }
cell *base_cons(SchemeY *s, cell *args) { return syO_cons(s, car(args), car(cdr(args))); }

cell *base_add(SchemeY *s, cell *args) {
  cell *r = syO_integer(s, 0);
  for (; args; args = cdr(args)) {
    cell *a = car(args);
    if (a->kind == TyInteger) {
      if (r->kind == TyInteger)
        r->as.integer += a->as.integer;
      else
        r->as.real += (double)a->as.integer;
    }
    else if (a->kind == TyReal) {
      if (r->kind == TyInteger) {
        r->kind = TyReal;
        r->as.real = (double)r->as.integer + a->as.real;
      }
      else r->as.real += a->as.real;
    }
    else return NULL;
  }
  return r;
}

cell *base_sub(SchemeY *s, cell *args) {
  cell *r = syO_integer(s, 0);
  for (; args; args = cdr(args)) {
    cell *a = car(args);
    if (a->kind == TyInteger) {
      if (r->kind == TyInteger)
        r->as.integer -= a->as.integer;
      else
        r->as.real -= (double)a->as.integer;
    }
    else if (a->kind == TyReal) {
      if (r->kind == TyInteger) {
        r->kind = TyReal;
        r->as.real = (double)r->as.integer - a->as.real;
      }
      else r->as.real -= a->as.real;
    }
    else return NULL;
  }
  return r;
}

cell *base_mul(SchemeY *s, cell *args) {
  cell *r, *a = car(args);
  if (a->kind == TyInteger)
    r = syO_integer(s, a->as.integer);
  else if (a->kind == TyReal)
    r = syO_real(s, a->as.real);
  else
    return NULL;
  for (args = cdr(args); args; args = cdr(args)) {
    a = car(args);
    if (a->kind == TyInteger) {
      if (r->kind == TyInteger)
        r->as.integer *= a->as.integer;
      else
        r->as.real *= (double)a->as.integer;
    }
    else if (a->kind == TyReal) {
      if (r->kind == TyInteger) {
        r->kind = TyReal;
        r->as.real = (double)r->as.integer * a->as.real;
      }
      else r->as.real *= a->as.real;
    }
    else return NULL;
  }
  return r;
}

cell *base_div(SchemeY *s, cell *args) {
  cell *r, *a = car(args);
  if (a->kind == TyInteger)
    r = syO_integer(s, a->as.integer);
  else if (a->kind == TyReal)
    r = syO_real(s, a->as.real);
  else
    return NULL;
  for (args = cdr(args); args; args = cdr(args)) {
    a = car(args);
    if (a->kind == TyInteger) {
      if (r->kind == TyInteger)
        r->as.integer /= a->as.integer;
      else
        r->as.real /= (double)a->as.integer;
    }
    else if (a->kind == TyReal) {
      if (r->kind == TyInteger) {
        r->kind = TyReal;
        r->as.real = (double)r->as.integer / a->as.real;
      }
      else r->as.real /= a->as.real;
    }
    else return NULL;
  }
  return r;
}

// Function application.
// cell *base_apply(SchemeY *s, cell *func, cell *args) {
//   switch (func->kind) {
//     case TyFFun:
//     case TyClosure:
//     default:
//       // ERROR
//       break;
//   }
// }

// ---------------------------------------------------------------------------
// unfiled
// ---------------------------------------------------------------------------

// Evaluate an expression.
// cell *eval(SchemeY *s, cell *expr, cell *env) {
//   if (expr == NULL)  // empty list
//     return NULL;
//   else if (expr->kind == TyPair) {  // application

//   }
//   else if (expr->kind == TySymbol) {  // lookup

//   }
//   else return expr;
// }

static void print_list(cell *expr) {
  print_expr(car(expr));
  if (cdr(expr)) {
    if (cdr(expr)->kind == TyPair) {
      fputc(' ', stdout);
      print_list(cdr(expr));
    }
    else {
      printf(" . ");
      print_expr(cdr(expr));
    }
  }
}

void print_expr(cell *expr) {
  if (expr == NULL)
    printf("()");
  else switch (expr->kind) {
    case TyInteger:
      printf("%ld", expr->as.integer);
      break;
    case TyReal:
      printf("%.3f", expr->as.real);
      break;
    case TyBoolean:
      printf(expr->as.integer ? "#t" : "#f");
      break;
    case TyString:
      printf("%s", expr->as.string);
      break;
    case TySymbol:
      printf("%s", expr->as.string);
      break;
    case TyPair:
      fputc('(', stdout);
      print_list(expr);
      fputc(')', stdout);
      break;
    case TyClosure:
      break;
    default:
      printf("()");
      break;
  }
}
