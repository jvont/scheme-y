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
        as(r).integer += as(a).integer;
      else
        as(r).real += (float)as(a).integer;
    }
    else if (isreal(a)) {
      if (isint(r))
        set_real(r, (float)as(r).integer + as(a).real);
      else
        as(r).real += as(a).real;
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
        as(r).integer -= as(a).integer;
      else
        as(r).real -= (float)as(a).integer;
    }
    else if (isreal(a)) {
      if (isint(r))
        set_real(r, (float)as(r).integer - as(a).real);
      else
        as(r).real -= as(a).real;
    }
    else return NULL;
  }
  return r;
}

cell_t *sy_mul(SchemeY *s, cell_t *args) {
  cell_t *r, *a = car(args);
  if (isint(a))
    r = mk_int(s, as(a).integer);
  else if (isreal(a))
    r = mk_real(s, as(a).real);
  else
    return NULL;
  for (args = cdr(args); args; args = cdr(args)) {
    a = car(args);
    if (isint(a)) {
      if (isint(r))
        as(r).integer *= as(a).integer;
      else
        as(r).real *= (float)as(a).integer;
    }
    else if (isreal(a)) {
      if (isint(r))
        set_real(r, (float)as(r).integer * as(a).real);
      else
        as(r).real *= as(a).real;
    }
    else return NULL;
  }
  return r;
}

cell_t *sy_div(SchemeY *s, cell_t *args) {
  cell_t *r, *a = car(args);
  if (isint(a))
    r = mk_int(s, as(a).integer);
  else if (isreal(a))
    r = mk_real(s, as(a).real);
  else
    return NULL;
  for (args = cdr(args); args; args = cdr(args)) {
    a = car(args);
    if (isint(a)) {
      if (isint(r))
        as(r).integer /= as(a).integer;
      else
        as(r).real /= (float)as(a).integer;
    }
    else if (isreal(a)) {
      if (isint(r))
        set_real(r, (float)as(r).integer / as(a).real);
      else
        as(r).real /= as(a).real;
    }
    else return NULL;
  }
  return r;
}
