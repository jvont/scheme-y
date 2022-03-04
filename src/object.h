/*
** Scheme object representation.
** Every scheme object is either an atom or cons cell. Atoms are tagged
** unions, while cons cells contain two pointers (car and cdr) to other
** Scheme objects. An atom's tag shares space with the car pointer of
** the cons cell, relying on the assumption that memory addresses will
** never reside within the first byte of the memory space.
*/
#ifndef _SY_OBJECT_H
#define _SY_OBJECT_H

#include "scheme-y.h"

#include <stdio.h>
#include <stdlib.h>

typedef union cell cell_t;

typedef cell_t *(ffun_t)(SchemeY *, cell_t *);

typedef struct vector {
  cell_t *items;
  size_t len, size;
} vector_t;

enum {
  INTEGER = 1,
  REAL,
  CHARACTER,
  STRING,
  SYMBOL,
  FFUN,
  CLOSURE,
  VECTOR,
  TABLE,
  PORT
};

union cell {
  struct {
    cell_t *car;
    cell_t *cdr;
  } cons;
  struct {
    size_t type;
    union {
      long integer;
      float real;
      int character;
      char *string;
      ffun_t *ffun;
      vector_t *vector;
      FILE *port;
    } as;
  } atom;
};

#define gett(c) ((c)->atom.type)
#define getv(c) ((c)->atom.as)

#define istype(c,t) (gett(c) == (t))
#define iscons(c) (gett(c) > 255 || car(c) == NULL)

#define sett(c,t) (gett(c) = (t))
#define setv(c,v) (getv(c) = (v))
#define set(c,t,v) (sett(c,t), setv(c,v))

#define car(c) ((c)->cons.car)
#define cdr(c) ((c)->cons.cdr)

// cell_t *cons(cell_t *car, cell_t *cdr) {
//   cell_t *p = malloc(sizeof(cell_t));
//   car(p) = car;
//   cdr(p) = cdr;
//   return p;
// }

cell_t *syO_integer   (SchemeY *s, long integer);
cell_t *syO_real      (SchemeY *s, double real);
cell_t *syO_character (SchemeY *s, int character);
cell_t *syO_string    (SchemeY *s, char *string);
cell_t *syO_symbol    (SchemeY *s, char *symbol);
cell_t *syO_ffun      (SchemeY *s, ffun_t *ffun);
cell_t *syO_cons      (SchemeY *s, cell_t *car, cell_t *cdr);
cell_t *syO_vector    (SchemeY *s, size_t size);
cell_t *syO_table     (SchemeY *s, size_t size);
cell_t *syO_port      (SchemeY *s, FILE *stream, char *mode);

// #define initobj(o,k) ((o)->kind = k, unmark(o))

// #define setint(o,i)      (initobj(o, TyInteger), (o)->as.integer = (i))
// #define setreal(o,r)     (initobj(o, TyReal), (o)->as.real = (r))
// #define setchar(o,c)     (initobj(o, TyCharacter), (o)->as.character = (c))
// #define setstring(o,s)   (initobj(o, TyString), (o)->as.string = (s))
// #define setffun(o,f)     (initobj(o, TyFFun), (o)->as.ffun = (f))
// #define setcons(o,a,d)   (initobj(o, TyPair), car(o) = (a), cdr(o) = (d))
// #define setvector(o,v,s) (initobj(o, TyVector), (o)->as.vector.items=v,
//                           (o)->as.vector.len = 0, (o)->as.vector.size = s)

cell_t *syO_read(SchemeY *s, cell_t *port);

void syO_print(cell_t *obj);
cell_t *syO_write(SchemeY *s, cell_t *args);

#endif