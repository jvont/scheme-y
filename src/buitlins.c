#include "builtins.h"
#include "state.h"

#include <math.h>
#include <stdlib.h>



// ---------------------------------------------------------------------------
// Lists
// ---------------------------------------------------------------------------

cell *syF_car(SchemeY *s, cell *arg) { return car(car(arg)); }
cell *syF_cdr(SchemeY *s, cell *arg) { return car(car(arg)); }
cell *syF_cons(SchemeY *s, cell *args) { return syO_cons(s, car(args), car(cdr(args))); }

// ---------------------------------------------------------------------------
// Numbers
// ---------------------------------------------------------------------------

cell *syB_add(SchemeY *s, cell *args) {
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

cell *syB_sub(SchemeY *s, cell *args) {
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

cell *syB_mul(SchemeY *s, cell *args) {
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

cell *syB_div(SchemeY *s, cell *args) {
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
