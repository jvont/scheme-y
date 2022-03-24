#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <stdio.h>

enum { TEST_PASS, TEST_FAIL };

typedef struct Test {
  int (*func)(void);
  char *name;
} Test;

#define _T2S(r) ((r) == TEST_PASS ? "PASSED" : "FAILED")

#define run_tests(table,label) {                          \
  int result = TEST_PASS;                                 \
  printf("%s:\n", label);                                 \
  size_t n_tests = sizeof(table) / sizeof(*table);        \
  for (size_t i = 0; i < n_tests; i++) {                  \
    Test t = table[i];                                    \
    printf("  [%zu/%zu] %s... ", i + 1, n_tests, t.name); \
    int r = t.func();                                     \
    if (result == TEST_PASS) { result = r; }              \
    printf("%s\n", _T2S(r));                              \
  }                                                       \
  printf("test result: %s\n\n", _T2S(result));            \
}

#endif