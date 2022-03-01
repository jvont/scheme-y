/*
** Scheme's object representation.
*/
#ifndef _SY_OBJECT_H
#define _SY_OBJECT_H

#include "scheme-y.h"

#include <stdio.h>
#include <stdbool.h>

#define car(c) ((c)->as.pair.car)
#define cdr(c) ((c)->as.pair.cdr)

typedef struct cell cell;
typedef cell *(ffun)(struct SchemeY *, cell *);

struct cell {
  enum {
    TyNil, TyInteger, TyReal, /* TyRational, TyComplex, */
    TyBoolean, TyCharacter, TyString, TySymbol, TyFFun,
    TyClosure, TyPair, TyVector, TyTable, TyPort
  } kind;
  union {
    long integer;
    double real;
    /* struct { long n; long d; } rational; */
    /* struct { double r; double i; } complex; */
    int character;
    char *string;
    ffun *ffun;
    struct { cell *car, *cdr; } pair;
    struct { cell *items; unsigned int length, size; } vector;
    struct { FILE *stream; char mode[3]; } port;
  } as;
};

cell *syO_kind (SchemeY *s, int kind);

cell *syO_integer   (SchemeY *s, long integer);
cell *syO_real      (SchemeY *s, double real);
cell *syO_character (SchemeY *s, int character);
cell *syO_string    (SchemeY *s, char *string);
cell *syO_symbol    (SchemeY *s, char *symbol);
cell *syO_ffun      (SchemeY *s, ffun *ffun);
cell *syO_cons      (SchemeY *s, cell *car, cell *cdr);
cell *syO_vector    (SchemeY *s, unsigned int size);
cell *syO_table    (SchemeY *s, unsigned int size);
cell *syO_port      (SchemeY *s, FILE *stream, char *mode);

// #define initobj(o,k) ((o)->kind = k, unmark(o))

// #define setint(o,i)      (initobj(o, TyInteger), (o)->as.integer = (i))
// #define setreal(o,r)     (initobj(o, TyReal), (o)->as.real = (r))
// #define setchar(o,c)     (initobj(o, TyCharacter), (o)->as.character = (c))
// #define setstring(o,s)   (initobj(o, TyString), (o)->as.string = (s))
// #define setffun(o,f)     (initobj(o, TyFFun), (o)->as.ffun = (f))
// #define setcons(o,a,d)   (initobj(o, TyPair), car(o) = (a), cdr(o) = (d))
// #define setvector(o,v,s) (initobj(o, TyVector), (o)->as.vector.items=v,\
//                           (o)->as.vector.len = 0, (o)->as.vector.size = s)

cell *syO_read(SchemeY *s, cell *arg);

void syO_print(cell *obj);
cell *syO_write(SchemeY *s, cell *args);

#endif