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
typedef struct heap heap_t;

typedef cell_t *(ffun_t)(SchemeY *, cell_t *);

typedef struct vector {
  cell_t *_items;
  size_t _len, _size;
} vector_t;

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
    size_t _type;  // _car-algined
    union {
      long _int;
      float _real;
      int _char;
      char *_string;
      ffun_t *_ffun;
      vector_t *_vector;
      FILE *_port;
    } _as;
  } _atom;
};

#define car(c) ((c)->_cons._car)
#define cdr(c) ((c)->_cons._cdr)

#define type(c) ((c)->_atom._type)
#define as(c)   ((c)->_atom._as)

#define iscons(c)   (type(c) > 255 || car(c) == NULL)
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

#define set_cons(c,a,d) (car(c) = (a), cdr(c) = (d))
#define set_int(c,i)    (type(c) = T_INT, as(c)._int = (i))
#define set_real(c,r)   (type(c) = T_REAL, as(c)._real = (r))
#define set_char(c,ch)  (type(c) = T_CHAR, as(c)._char = (ch))
#define set_string(c,s) (type(c) = T_STRING, as(c)._string = (s))
#define set_symbol(c,s) (type(c) = T_SYMBOL, as(c)._string = (s))
#define set_ffun(c,f)   (type(c) = T_FFUN,   as(c)._ffun = (f))
#define set_vector(c,v) (type(c) = T_VECTOR, as(c)._vector = (v))
#define set_table(c,v)  (type(c) = T_TABLE, as(c)._vector = (v))
#define set_port(c,p)   (type(c) = T_PORT,   as(c)._port = (p))

cell_t *cons(SchemeY *s, cell_t *a, cell_t *d);
cell_t *mk_int(SchemeY *s, long i);
cell_t *mk_real(SchemeY *s, float r);
cell_t *mk_char(SchemeY *s, int ch);
cell_t *mk_string(SchemeY *s, char *str);
cell_t *mk_symbol(SchemeY *s, char *sym);
cell_t *mk_ffun(SchemeY *s, ffun_t *f);
vector_t *mk_vector_t(SchemeY *s, size_t sz);
cell_t *mk_vector(SchemeY *s, size_t sz);
cell_t *mk_table(SchemeY *s, size_t sz);
cell_t *mk_port(SchemeY *s, FILE *p);

#endif