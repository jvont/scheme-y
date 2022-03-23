#include "builtins.h"
#include "state.h"

#include <math.h>
#include <stdlib.h>

// Cell *sy_car(State *s, Cell *arg) { return car(car(arg)); }
// Cell *sy_cdr(State *s, Cell *arg) { return car(car(arg)); }
// Cell *sy_cons(State *s, Cell *args) { return cons(s, car(args), car(cdr(args))); }

Cell *sy_add(State *s, Cell *args) {
  Cell *r = mk_integer(0);
  for (; args; args = cdr(args)) {
    Cell *a = car(args);
    if (isinteger(a)) {
      if (isinteger(r))
        as(r).integer += as(a).integer;
      else
        as(r).real += (float)as(a).integer;
    }
    else if (isreal(a)) {
      if (isinteger(r))
        set_real(r, (float)as(r).integer + as(a).real);
      else
        as(r).real += as(a).real;
    }
    else return NULL;
  }
  return r;
}

Cell *sy_sub(State *s, Cell *args) {
  Cell *r = mk_integer(0);
  for (; args; args = cdr(args)) {
    Cell *a = car(args);
    if (isinteger(a)) {
      if (isinteger(r))
        as(r).integer -= as(a).integer;
      else
        as(r).real -= (float)as(a).integer;
    }
    else if (isreal(a)) {
      if (isinteger(r))
        set_real(r, (float)as(r).integer - as(a).real);
      else
        as(r).real -= as(a).real;
    }
    else return NULL;
  }
  return r;
}

Cell *sy_mul(State *s, Cell *args) {
  Cell *r, *a = car(args);
  if (isinteger(a))
    r = mk_integer(as(a).integer);
  else if (isreal(a))
    r = mk_real(as(a).real);
  else
    return NULL;
  for (args = cdr(args); args; args = cdr(args)) {
    a = car(args);
    if (isinteger(a)) {
      if (isinteger(r))
        as(r).integer *= as(a).integer;
      else
        as(r).real *= (float)as(a).integer;
    }
    else if (isreal(a)) {
      if (isinteger(r))
        set_real(r, (float)as(r).integer * as(a).real);
      else
        as(r).real *= as(a).real;
    }
    else return NULL;
  }
  return r;
}

Cell *sy_div(State *s, Cell *args) {
  Cell *r, *a = car(args);
  if (isinteger(a))
    r = mk_integer(as(a).integer);
  else if (isreal(a))
    r = mk_real(as(a).real);
  else
    return NULL;
  for (args = cdr(args); args; args = cdr(args)) {
    a = car(args);
    if (isinteger(a)) {
      if (isinteger(r))
        as(r).integer /= as(a).integer;
      else
        as(r).real /= (float)as(a).integer;
    }
    else if (isreal(a)) {
      if (isinteger(r))
        set_real(r, (float)as(r).integer / as(a).real);
      else
        as(r).real /= as(a).real;
    }
    else return NULL;
  }
  return r;
}
