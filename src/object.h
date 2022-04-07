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
** of 8 bytes (for 32-bit systems). This allows us to safely check the type
** of an atom without needing to verify using isatom.
**
** Both vectors and tables use the same vector object to store values.
*/
#ifndef OBJECT_H
#define OBJECT_H

#include "scheme-y.h"

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

typedef struct Object Object;
typedef struct Vector Vector;

struct Object {
  union {
    enum {  // Atom types (must be odd-numbered)
      T_NIL       = 1,
      T_INTEGER   = 3,
      T_REAL      = 5,
      T_BOOLEAN   = 7,
      T_CHARACTER = 9,
      T_STRING    = 11,
      T_SYMBOL    = 13,
      T_CCLOSURE  = 15,
      T_SCLOSURE  = 17,
      T_VECTOR    = 19,
      T_TABLE     = 21,
      T_PORT      = 23,
      T_FWD       = 25  // forward heap reference
    } _tt;
    Object *_car;
  } _fst;
  union {
    int32_t integer;
    float real;
    uint32_t character;
    char *string;
    SyFunction *ffunction;
    Vector *vector;
    FILE *port;
    Object *fwd;  // also stores cdr address
  } _snd;
} __attribute__((aligned(8)));

struct Vector {
  size_t len, size;
  Object items[];  // flexible array member (C99)
};

// Is the first bit set?
#define isfbs(x) ((x)->_fst._tt & 0x1)

#define islist(x) (isfbs(x) == 0x0)
#define car(x) ((x)->_fst._car)
#define cdr(x) ((x)->_snd.fwd)

#define isatom(x) (isfbs(x) == 0x1)
#define type(x) ((x)->_fst._tt)
#define as(x)   ((x)->_snd)

#endif