/*
** Simple tests to make sure some of our more tricky macros are
** functioning correctly.
*/

#include "../src/object.h"

#include <assert.h>
#include <stdio.h>

int main() {
  if (sizeof(List) != sizeof(Atom))
    fprintf(stderr, "Warning: List size != Atom size.\n");

  Object x, _car, _cdr;
  x.list._car = &_car;
  x.list._cdr = &_cdr;
  Object *xref = (Object *)tag(&x);

  assert(islist(xref));
  assert(car(xref) == &_car);
  assert(cdr(xref) == &_cdr);

  x.atom.type = T_INTEGER;
  x.atom.as.integer = 5;
  xref = (Object *)untag(xref);

  assert(isatom(xref));
  assert(type(xref) == T_INTEGER);
  assert(as(xref).integer == 5);

  return 0;
}
