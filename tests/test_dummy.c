#include "testlib.h"

int dummy_pass() { return TEST_PASS; }
int dummy_fail() { return TEST_FAIL; }

Test table[] = {
  { dummy_pass, "dummy pass" },
  { dummy_fail, "dummy fail" }
};

int main() {
  run_tests(table, "dummy test");
  return 0;
}
