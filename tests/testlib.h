/*
** Test utility functions. Tests are given as a static array of Test
** objects, where each test function returns either TEST_PASS or TEST_FAIL.
*/
#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <stdio.h>

enum { TEST_PASS, TEST_FAIL };

typedef struct Test {
  int (*func)(void);
  char *name;
} Test;

#define run_tests(table,label) {                    \
  printf("\x1b[33mTEST\x1b[0m: %s\n", label);       \
  size_t n = sizeof(table) / sizeof(*table);        \
  for (size_t i = 0; i < n; i++) {                  \
    Test t = table[i];                              \
    printf("  [%zu/%zu] %s... ", i + 1, n, t.name); \
    fflush(stdout);                                 \
    int r = t.func();                               \
    if (r) { printf("\x1b[31mFAILED\x1b[0m\n"); }   \
    else { printf("\x1b[32mPASSED\x1b[0m\n"); }     \
  }                                                 \
}

#endif