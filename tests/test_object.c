/*
** Simple tests to make sure some of our more tricky macros are
** functioning correctly.
*/
#include "testlib.h"
#include "../src/object.h"

int test_object_size() {
  return sizeof(Object) == 2 * sizeof(void *) ? TEST_PASS : TEST_FAIL;
}

int test_islist() {
  Object x;
  x.list = (List) { ._car = NULL, ._cdr = NULL };
  return islist(&x) ? TEST_PASS : TEST_FAIL;
}

int test_islist_atom() {
  Object x;
  x.list = (List) { ._car = NULL, ._cdr = NULL };
  return islist(&x) ? TEST_PASS : TEST_FAIL;
}

int test_car_cdr() {
  Object x;
  Object _car;
  x.list._car = &_car;
  return car(&x) == &_car ? TEST_PASS : TEST_FAIL;
}

int test_isatom() {
  Object x;
  x.atom = (Atom) { .type = T_INTEGER, .as.integer = 5 };
  return isatom(&x) ? TEST_PASS : TEST_FAIL;
}

int test_isatom_list() {
  Object x;
  x.atom = (Atom) { .type = T_INTEGER, .as.integer = 5 };
  return isatom(&x) ? TEST_PASS : TEST_FAIL;
}

int test_type() {
  Object x;
  x.atom.type = T_INTEGER;
  x.atom.as.integer = 5;
  return type(&x) == T_INTEGER ? TEST_PASS : TEST_FAIL;
}

int test_as() {
  Object x;
  x.atom.type = T_INTEGER;
  x.atom.as.integer = 5;
  return as(&x).integer == 5 ? TEST_PASS : TEST_FAIL;
}

Test table[] = {
  { test_object_size, "object size = 2 * pointer size" },
  { test_islist, "list -> islist" },
  { test_islist_atom, "atom -> islist" },
  { test_car_cdr, "car/cdr field access" },
  { test_isatom, "atom -> isatom" },
  { test_isatom_list, "list -> isatom" },
  { test_type, "type field access" },
  { test_as, "as field access" }
};

int main() {
  run_tests(table, "test object.h macros")
  return 0;
}
