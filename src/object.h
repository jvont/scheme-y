/*
** Scheme object representation.
**
** Scheme contains two types of objects: lists and atoms. Lists contain two
** pointers (car and cdr) to other Scheme objects, while atoms are tagged
** unions containing a variety of different types.
**
** The type tag of an atom and car pointer of a list cell share the same
** memory. To differentiate between the two, all atom tags are odd-numbered
** (have their LSB set), while all list cells have a minimum memory alignment
** of 8 bytes (for 32-bit systems).
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

enum {
  T_INTEGER = 1,
  T_REAL = 3,
  T_CHARACTER = 5,
  T_STRING = 7,
  T_PROCEDURE = 9,
  T_SYNTAX = 11,
  T_VECTOR = 13,
  T_TABLE = 15,
  T_PORT = 17,
  T_FWD = 19
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

// Is the first bit set?
#define isfbs(x) (((x)->atom.type) & 0x1)

#define isatom(x) (isfbs(x) == 0x1)
#define type(x) (((Atom *)(x))->type)
#define as(x)   (((Atom *)(x))->as)

#define islist(x) (isfbs(x) == 0x0)
#define car(x) (((List *)(x))->_car)
#define cdr(x) (((List *)(x))->_cdr)

#endif