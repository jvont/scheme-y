#include "builtins.h"
#include "state.h"

#include <math.h>
#include <stdlib.h>

// cell_t *sy_car(SchemeY *s, cell_t *arg) { return car(car(arg)); }
// cell_t *sy_cdr(SchemeY *s, cell_t *arg) { return car(car(arg)); }
// cell_t *sy_cons(SchemeY *s, cell_t *args) { return cons(s, car(args), car(cdr(args))); }

cell_t *sy_add(SchemeY *s, cell_t *args) {
  cell_t *r = mk_int(s, 0);
  for (; args; args = cdr(args)) {
    cell_t *a = car(args);
    if (isint(a)) {
      if (isint(r))
        as(r)._int += as(a)._int;
      else
        as(r)._real += (float)as(a)._int;
    }
    else if (isreal(a)) {
      if (isint(r))
        set_real(r, (float)as(r)._int + as(a)._real);
      else
        as(r)._real += as(a)._real;
    }
    else return NULL;
  }
  return r;
}

cell_t *sy_sub(SchemeY *s, cell_t *args) {
  cell_t *r = mk_int(s, 0);
  for (; args; args = cdr(args)) {
    cell_t *a = car(args);
    if (isint(a)) {
      if (isint(r))
        as(r)._int -= as(a)._int;
      else
        as(r)._real -= (float)as(a)._int;
    }
    else if (isreal(a)) {
      if (isint(r))
        set_real(r, (float)as(r)._int - as(a)._real);
      else
        as(r)._real -= as(a)._real;
    }
    else return NULL;
  }
  return r;
}

cell_t *sy_mul(SchemeY *s, cell_t *args) {
  cell_t *r, *a = car(args);
  if (isint(a))
    r = mk_int(s, as(a)._int);
  else if (isreal(a))
    r = mk_real(s, as(a)._real);
  else
    return NULL;
  for (args = cdr(args); args; args = cdr(args)) {
    a = car(args);
    if (isint(a)) {
      if (isint(r))
        as(r)._int *= as(a)._int;
      else
        as(r)._real *= (float)as(a)._int;
    }
    else if (isreal(a)) {
      if (isint(r))
        set_real(r, (float)as(r)._int * as(a)._real);
      else
        as(r)._real *= as(a)._real;
    }
    else return NULL;
  }
  return r;
}

cell_t *sy_div(SchemeY *s, cell_t *args) {
  cell_t *r, *a = car(args);
  if (isint(a))
    r = mk_int(s, as(a)._int);
  else if (isreal(a))
    r = mk_real(s, as(a)._real);
  else
    return NULL;
  for (args = cdr(args); args; args = cdr(args)) {
    a = car(args);
    if (isint(a)) {
      if (isint(r))
        as(r)._int /= as(a)._int;
      else
        as(r)._real /= (float)as(a)._int;
    }
    else if (isreal(a)) {
      if (isint(r))
        set_real(r, (float)as(r)._int / as(a)._real);
      else
        as(r)._real /= as(a)._real;
    }
    else return NULL;
  }
  return r;
}
