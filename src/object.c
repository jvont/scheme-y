#include "object.h"
#include "heap.h"

#include <string.h>

// Object *mk_cons(Object *_car, Object *_cdr) {
//   Object *x = mem_malloc(sizeof(Object));
//   car(x) = _car;
//   cdr(x) = _cdr;
//   return (Object *)tag(x);
// }

// Object *mk_integer(long i) {
//   Object *x = mem_malloc(sizeof(Object));
//   type(x) = T_INTEGER;
//   as(x).integer = i;
//   return x;
// }

// Object *mk_real(float r) {
//   Object *x = mem_malloc(sizeof(Object));
//   type(x) = T_REAL;
//   as(x).real = r;
//   return x;
// }

// Object *mk_character(int c) {
//   Object *x = mem_malloc(sizeof(Object));
//   type(x) = T_CHARACTER;
//   as(x).character = c;
//   return x;
// }

// Object *mk_string(const char *s) {
//   Object *x = mem_malloc(sizeof(Object));
//   type(x) = T_STRING;
//   as(x).string = mem_strdup(s);
//   return x;
// }

// Object *mk_symbol(const char *s) {
//   Object *x = mem_malloc(sizeof(Object));
//   type(x) = T_SYMBOL;
//   as(x).string = mem_strdup(s);
//   return x;
// }

// Object *mk_procedure(Procedure *p) {
//   Object *x = mem_malloc(sizeof(Object));
//   type(x) = T_PROCEDURE;
//   as(x).procedure = p;
//   return x;
// }

// Object *mk_syntax(Syntax *s) {
//   Object *x = mem_malloc(sizeof(Object));
//   type(x) = T_SYNTAX;
//   as(x).syntax = s;
//   return x;
// }

// static Vector *vvector(size_t n) {
//   Vector *v = mem_calloc(n + objsize(sizeof(Vector)), sizeof(Object));
//   v->len = 0;
//   v->size = n;
//   return v;
// }

// Object *mk_vector(size_t n) {
//   Object *x = mem_malloc(sizeof(Object));
//   type(x) = T_VECTOR;
//   as(x).vector = vvector(n);
//   return x;
// }

// Object *mk_table(size_t n) {
//   Object *x = mem_malloc(sizeof(Object));
//   type(x) = T_TABLE;
//   as(x).vector = vvector(n);
//   return x;
// }

// Object *mk_port(FILE *p) {
//   Object *x = mem_malloc(sizeof(Object));
//   type(x) = T_PORT;
//   as(x).port = p;
//   return x;
// }
