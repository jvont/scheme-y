/*
** Scheme object representation.
** Scheme contains two types of objects: lists and atoms. The LSB of object
** pointers is tagged to indicate whether or not it is a list. Lists contain
** two pointers (car and cdr) to other Scheme objects, while atoms are tagged
** unions containing a variety of different types.
*/
#ifndef _OBJECT_H
#define _OBJECT_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct State State;
typedef union Object Object;
typedef struct Vector Vector;

typedef Object *(ffun_t)(State *, Object *);

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
    T_INTEGER, T_REAL, // T_RATIONAL, T_COMPLEX,
    T_CHARACTER,
    T_STRING, T_SYMBOL,
    T_FFUN, // T_PRIMITIVE, T_SYNTAX,
    T_VECTOR, T_TABLE,
    T_PORT,
    T_FWD
  } type;
  union {
    int32_t integer;
    float real;
    uint32_t character;
    char *string;
    ffun_t *ffun;
    Vector *vector;
    FILE *port;
    Object *fwd;
  } as;
} Atom;

union Object {
  List list;
  Atom atom;
};

/* Header for a dynamic array */
struct Vector {
  size_t len, size;  /* should be same size as Cell */
  Object items[];  /* variable-sized array (C99) */
};

#define tag(x)   ((size_t)x | 0x1)
#define untag(x) ((size_t)x & ~0x1)

#define islist(x) (((size_t)x & 0x1) == 0x1)
#define isatom(x) (((size_t)x & 0x1) == 0x0)

#define car(x) (((List *)untag(x))->_car)
#define cdr(x) (((List *)untag(x))->_cdr)

#define type(x) (((Atom *)(x))->type)
#define as(x)   (((Atom *)(x))->as)

#define isinteger(x)   (isatom(x) && type(x) == T_INTEGER)
#define isreal(x)      (isatom(x) && type(x) == T_REAL)
#define isnumber(x)    (isatom(x) && (isinteger(x) || isreal(x)))
#define ischaracter(x) (isatom(x) && type(x) == T_CHARACTER)
#define isstring(x)    (isatom(x) && type(x) == T_STRING)
#define issymbol(x)    (isatom(x) && type(x) == T_SYMBOL)
#define isffun(x)      (isatom(x) && type(x) == T_FFUN)
#define isvector(x)    (isatom(x) && type(x) == T_VECTOR)
#define istable(x)     (isatom(x) && type(x) == T_TABLE)
#define isport(x)      (isatom(x) && type(x) == T_PORT)
#define isfwd(x)       (isatom(x) && type(x) == T_FWD)

#define set_int(x,i)    (type(x) = T_INTEGER, as(x).integer = i)
#define set_real(x,r)   (type(x) = T_REAL, as(x).real = r)

Object *mk_cons(Object *a, Object *d);
Object *mk_integer(long i);
Object *mk_real(float r);
Object *mk_character(int c);
Object *mk_string(const char *s);
Object *mk_symbol(const char *s);
Object *mk_ffun(ffun_t *f);
Object *mk_vector(size_t n);
Object *mk_table(size_t n);
Object *mk_port(FILE *p);

#endif