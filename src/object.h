/*
** Scheme object representation.
** Every scheme object is either an atom or cons cell. Atoms are tagged
** unions, while cons cells contain two pointers (car and cdr) to other
** Scheme objects. An atom's tag shares space with the car pointer of
** the cons cell, relying on the assumption that memory addresses below
** 16 are unused.
*/
#ifndef _SY_OBJECT_H
#define _SY_OBJECT_H

#include "scheme-y.h"

#include <stdio.h>
#include <stdlib.h>

typedef union cell cell_t;

typedef cell_t *(ffun_t)(SchemeY *, cell_t *);

typedef struct vector {
  cell_t *_items;
  size_t _len, _size;
} vector_t;

// enum {
//   P_FREE = 0
//   P_FILE = 1,
//   P_BUFFER = 2,
//   P_READ = 4,
//   P_WRITE = 8,
//   P_EOF = 16,
// };

// typedef struct port {
//   unsigned int type;
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
//   } as;
// } port_t;

enum {
  T_NIL,
  T_INT,
  T_REAL,
  T_CHAR,
  T_STRING,
  T_SYMBOL,
  T_FFUN,
  T_CLOSURE,
  T_VECTOR,
  T_TABLE,
  T_PORT
};

union cell {
  struct {
    cell_t *_car;
    cell_t *_cdr;
  } _cons;
  struct {
    size_t _type;  // _car-aligned
    union {
      long integer;
      float real;
      int character;
      char *string;
      ffun_t *ffun;
      vector_t *vector;
      FILE *port;
    } _as;
  } _atom;
};

#define car(c) ((c)->_cons._car)
#define cdr(c) ((c)->_cons._cdr)

#define type(c) ((c)->_atom._type)
#define as(c)   ((c)->_atom._as)

#define iscons(c)   (type(c) > 255 || car(c) == NULL)
#define isatom(c)   (type(c) <= T_PORT && type(c) > 0)
#define isint(c)    (type(c) == T_INT)
#define isreal(c)   (type(c) == T_REAL)
#define isnumber(c) (isint(c) || isreal(c))
#define ischar(c)   (type(c) == T_CHAR)
#define isstring(c) (type(c) == T_STRING)
#define issymbol(c) (type(c) == T_SYMBOL)
#define isffun(c)   (type(c) == T_FFUN)
#define isvector(c) (type(c) == T_VECTOR)
#define istable(c)  (type(c) == T_TABLE)
#define isport(c)   (type(c) == T_PORT)

cell_t *set_cons(cell_t *c, cell_t *_car, cell_t *_cdr);
cell_t *set_int(cell_t *c, long i);
cell_t *set_real(cell_t *c, float r);
cell_t *set_char(cell_t *c, int ch);
cell_t *set_string(cell_t *c, char *s);
cell_t *set_symbol(cell_t *c, char *s);
cell_t *set_ffun(cell_t *c, ffun_t *f);
cell_t *set_vector(cell_t *c, vector_t *v);
cell_t *set_table(cell_t *c, vector_t *v);
cell_t *set_port(cell_t *c, FILE *p);

cell_t *cons(SchemeY *s, cell_t *a, cell_t *d);
cell_t *mk_int(SchemeY *s, long i);
cell_t *mk_real(SchemeY *s, float r);
cell_t *mk_char(SchemeY *s, int ch);
cell_t *mk_string(SchemeY *s, char *str);
cell_t *mk_symbol(SchemeY *s, char *sym);
cell_t *mk_ffun(SchemeY *s, ffun_t *f);
cell_t *mk_vector(SchemeY *s, size_t sz);
cell_t *mk_table(SchemeY *s, size_t sz);
cell_t *mk_port(SchemeY *s, FILE *p);

vector_t *mk_vector_t(SchemeY *s, size_t sz);

#endif