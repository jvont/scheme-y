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
  return islist(&x) ? TEST_FAIL : TEST_PASS;
}

int test_islist_tag() {
  Object x;
  Object *xref = (Object *)tag(&x);
  return islist(xref) ? TEST_PASS : TEST_FAIL;
}

int test_car_cdr() {
  Object x, _car;
  x.list._car = &_car;
  Object *xref = (Object *)tag(&x);
  return car(xref) == &_car ? TEST_PASS : TEST_FAIL;
}

int test_isatom() {
  Object x;
  x.atom.type = T_INTEGER;
  x.atom.as.integer = 5;
  return isatom(&x) ? TEST_PASS : TEST_FAIL;
}

int test_isatom_tag() {
  Object x;
  Object *xref = (Object *)tag(&x);
  return isatom(xref) ? TEST_FAIL : TEST_PASS;
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
  { test_islist, "islist() for untagged object" },
  { test_islist_tag, "islist() for tagged object" },
  { test_car_cdr, "car field access for tagged object" },
  { test_isatom, "isatom() for untagged object" },
  { test_isatom_tag, "isatom() for tagged object" },
  { test_type, "type field access" },
  { test_as, "as field access" }
};

int main() {
  run_tests(table, "test object.h macros")
  return 0;
}
