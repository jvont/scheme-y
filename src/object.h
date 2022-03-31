/*
** Scheme object representation.
**
** Scheme contains two types of objects: lists and atoms. Lists contain two
** pointers (car and cdr) to other Scheme objects, while atoms are tagged
** unions containing a variety of different types.
**
** The type tag of an atom and car pointer of a list cell share the same
** memory. To differentiate between the two, all atom tags have their LSB set
** while list cells have a minimum memory alignment of 8 bytes.
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

#define tbit(x) ((1 << (x)) | 0x1)

enum {
  T_INTEGER = tbit(0),
  T_REAL = tbit(1),
  T_CHARACTER = tbit(2),
  T_STRING = tbit(3),
  T_PROCEDURE = tbit(4),
  T_SYNTAX = tbit(5),
  T_VECTOR = tbit(6),
  T_TABLE = tbit(7),
  T_PORT = tbit(8),
  T_FWD = tbit(9)
  // T_RATIONAL,
  // T_COMPLEX,
};

typedef struct Atom {
  uintptr_t type;  // _car-aligned
  union {
    int32_t integer;
    float real;
    uint32_t character;
    char *string;
    Procedure *procedure;
    Syntax *syntax;
    Vector *vector;
    FILE *port;
    Object *fwd;
  } as;
} Atom;

// C99 - section 6.7.2.1, paragraph 14: A pointer to a union object, suitably
// converted, points to each of its members [...], and vice versa.
union Object {
  Atom atom;
  List list;
} __attribute__((aligned(8)));

struct Vector {
  size_t len, size;
  Object items[];  // flexible array member (C99)
};

// #define cast_p2i(p) ((uintptr_t)((void *)(p)))

// Is the first bit set?
#define isfbs(x) (((x)->atom.type) & 0x1)

#define isatom(x) (isfbs(x) == 0x1)
#define type(x) (((Atom *)(x))->type)
#define as(x)   (((Atom *)(x))->as)

#define islist(x) (isfbs(x) == 0x0)
#define car(x) (((List *)(x))->_car)
#define cdr(x) (((List *)(x))->_cdr)

#endif