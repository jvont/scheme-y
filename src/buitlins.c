#include "builtins.h"
#include "state.h"

#include <math.h>
#include <stdlib.h>



// ---------------------------------------------------------------------------
// Lists
// ---------------------------------------------------------------------------

cell_t *syF_car(SchemeY *s, cell_t *arg) { return car(car(arg)); }
cell_t *syF_cdr(SchemeY *s, cell_t *arg) { return car(car(arg)); }
cell_t *syF_cons(SchemeY *s, cell_t *args) { return syO_cons(s, car(args), car(cdr(args))); }

// ---------------------------------------------------------------------------
// Numbers
// ---------------------------------------------------------------------------

cell_t *syB_add(SchemeY *s, cell_t *args) {
  cell_t *r = syO_integer(s, 0);
  for (; args; args = cdr(args)) {
    cell_t *a = car(args);
    if (gett(a) == INTEGER) {
      if (gett(r) == INTEGER)
        getv(r).integer += getv(a).integer;
      else
        getv(r).real += (double)getv(a).integer;
    }
    else if (gett(a) == REAL) {
      if (gett(r) == INTEGER) {
        gett(r) = REAL;
        getv(r).real = (double)getv(r).integer + getv(a).real;
      }
      else getv(r).real += getv(a).real;
    }
    else return NULL;
  }
  return r;
}

cell_t *syB_sub(SchemeY *s, cell_t *args) {
  cell_t *r = syO_integer(s, 0);
  for (; args; args = cdr(args)) {
    cell_t *a = car(args);
    if (gett(a) == INTEGER) {
      if (gett(r) == INTEGER)
        getv(r).integer -= getv(a).integer;
      else
        getv(r).real -= (double)getv(a).integer;
    }
    else if (gett(a) == REAL) {
      if (gett(r) == INTEGER) {
        gett(r) = REAL;
        getv(r).real = (double)getv(r).integer - getv(a).real;
      }
      else getv(r).real -= getv(a).real;
    }
    else return NULL;
  }
  return r;
}

cell_t *syB_mul(SchemeY *s, cell_t *args) {
  cell_t *r, *a = car(args);
  if (gett(a) == INTEGER)
    r = syO_integer(s, getv(a).integer);
  else if (gett(a) == REAL)
    r = syO_real(s, getv(a).real);
  else
    return NULL;
  for (args = cdr(args); args; args = cdr(args)) {
    a = car(args);
    if (gett(a) == INTEGER) {
      if (gett(r) == INTEGER)
        getv(r).integer *= getv(a).integer;
      else
        getv(r).real *= (double)getv(a).integer;
    }
    else if (gett(a) == REAL) {
      if (gett(r) == INTEGER) {
        gett(r) = REAL;
        getv(r).real = (double)getv(r).integer * getv(a).real;
      }
      else getv(r).real *= getv(a).real;
    }
    else return NULL;
  }
  return r;
}

cell_t *syB_div(SchemeY *s, cell_t *args) {
  cell_t *r, *a = car(args);
  if (gett(a) == INTEGER)
    r = syO_integer(s, getv(a).integer);
  else if (gett(a) == REAL)
    r = syO_real(s, getv(a).real);
  else
    return NULL;
  for (args = cdr(args); args; args = cdr(args)) {
    a = car(args);
    if (gett(a) == INTEGER) {
      if (gett(r) == INTEGER)
        getv(r).integer /= getv(a).integer;
      else
        getv(r).real /= (double)getv(a).integer;
    }
    else if (gett(a) == REAL) {
      if (gett(r) == INTEGER) {
        gett(r) = REAL;
        getv(r).real = (double)getv(r).integer / getv(a).real;
      }
      else getv(r).real /= getv(a).real;
    }
    else return NULL;
  }
  return r;
}
