/*
** Utility functions.
*/
#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

#define MAX(A,B) (((A) > (B)) ? (A) : (B))
#define MIN(A,B) (((A) < (B)) ? (A) : (B))

void *err_malloc(size_t size);
void *err_calloc(size_t n, size_t size);
void *err_realloc(void *ptr, size_t size);

#endif