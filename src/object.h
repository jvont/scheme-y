/*
** Scheme object representation.
**
** Scheme contains two types of objects: lists and atoms. The LSB of object
** pointers is tagged to indicate whether or not it is a list. Lists contain
** two pointers (car and cdr) to other Scheme objects, while atoms are tagged
** unions containing a variety of different types.
**
** Both vectors and tables use the same dynamic array object under the hood.
**
** Syntax functions (ex. quote, lambda) accept a lexical environment as a
** second argument. The arguments list passed to syntax functions is also
** passed unevaluated.
*/
#ifndef OBJECT_H
#define OBJECT_H

#include "scheme-y.h"

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

typedef struct Vector Vector;

// typedef struct Port {
//   enum {
//     P_FREE = 0,
//     P_FILE = 1,
//     P_BUFFER = 2,
//     P_READ = 4,
//     P_WRITE = 8,
//     P_EOF = 16,
//   } _type;
//   union {
//     struct {
//       FILE *stream;
//       char *name;
//     } file;
//     struct {
//       char *base;
//       char *ptr;
//       char *end;
//     } buffer;
//   } _as;
// } Port;

typedef struct List {
  Object *_car;
  Object *_cdr;
} List;

typedef struct Atom {
  enum {
    T_INTEGER, T_REAL, /* T_RATIONAL, T_COMPLEX, */
    T_CHARACTER,
    T_STRING, T_SYMBOL,
    T_PROCEDURE, T_SYNTAX,
    T_VECTOR, T_TABLE,
    T_PORT
  } type;
  union {
    int32_t integer;
    float real;
    uint32_t character;
    char *string;
    Procedure *procedure;
    Syntax *syntax;
    Vector *vector;
    FILE *port;
  } as;
} Atom;

// C99 - section 6.7.2.1, paragraph 14: A pointer to a union object, suitably
// converted, points to each of its members [...], and vice versa.
union Object {
  List list;
  Atom atom;
};

struct Vector {
  Object *items;
  size_t len, size;
};

#define cast_p2i(p) ((uintptr_t)((void *)(p)))

#define tag(x)   (cast_p2i(x) | 0x1)
#define untag(x) (cast_p2i(x) & ~0x1)

#define islist(x) ((cast_p2i(x) & 0x1) == 0x1)
#define car(x) (((List *)untag(x))->_car)
#define cdr(x) (((List *)untag(x))->_cdr)

#define isatom(x) ((cast_p2i(x) & 0x1) == 0x0)
#define type(x) (((Atom *)(x))->type)
#define as(x)   (((Atom *)(x))->as)

#endif