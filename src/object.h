/*
** Scheme object representation.
** Every scheme cell contains either an atom or list. Atoms are tagged
** unions, while cons cells contain two pointers (car and cdr) to other
** Scheme objects. An atom's tag shares space with the car pointer of
** the cons cell, relying on the assumption that memory addresses below
** 16 are unused.
*/
#ifndef _OBJECT_H
#define _OBJECT_H

#include <stdio.h>
#include <stdlib.h>

typedef struct SchemeY_ SchemeY;
typedef union cell_ cell;
typedef struct vector_ vector;

typedef cell *(ffun_t)(SchemeY *, cell *);

// typedef struct port_ {
//   enum {
//     P_FREE = 0,
//     P_FILE = 1,
//     P_BUFFER = 2,
//     P_READ = 4,
//     P_WRITE = 8,
//     P_EOF = 16,
//   } type;
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
// } port;

enum {
  T_INT = 1,
  T_REAL,
  T_CHAR,
  T_STRING,
  T_SYMBOL,
  T_FFUN,
  T_VECTOR,
  T_TABLE,
  T_PORT,
  T_FWD,  /* forwarding pointer */
};

union cell_ {
  struct {
    cell *car;
    cell *cdr;
  } list;
  struct {
    size_t type;  /* car-aligned */
    union {
      long integer;
      float real;
      int character;
      char string[1];  /* store in cdr space */
      ffun_t *ffun;
      vector *vector;
      FILE *port;
      cell *fwd;
    } as;
  } atom;
};

struct vector_ {
  size_t len, size;
  cell items[];  /* variable-sized array (C99) */
};

// #define get(c) (!(c) || (c)->atom.type != T_FWD ? (c) : (c)->atom.as.fwd)

#define car(c) ((c)->list.car)
#define cdr(c) ((c)->list.cdr)

#define type(c) ((c)->atom.type)
#define as(c)   ((c)->atom.as)

#define islist(c)   (c && type(c) > 255)
#define isatom(c)   (!c || (T_INT <= type(c) && type(c) <= T_PORT))

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
#define isfwd(c)    (type(c) == T_FWD)

#define set_cons(c,a,d) (car(c) = a, cdr(c) = d)
#define set_int(c,i)    (type(c) = T_INT, as(c).integer = i)
#define set_real(c,r)   (type(c) = T_REAL, as(c).real = r)
#define set_char(c,ch)  (type(c) = T_CHAR, as(c).character = ch)
#define set_string(c,s) (type(c) = T_STRING, as(c).string = s)
#define set_symbol(c,s) (type(c) = T_SYMBOL, as(c).string = s)
#define set_ffun(c,f)   (type(c) = T_FFUN, as(c).ffun = f)
#define set_vector(c,v) (type(c) = T_VECTOR, as(c).vector = v)
#define set_table(c,v)  (type(c) = T_TABLE, as(c).vector = v)
#define set_port(c,p)   (type(c) = T_PORT, as(c).port = p)
// cell *set_fwd(cell *c, cell *f);

cell *cons(SchemeY *s, cell *a, cell *d);
cell *mk_int(SchemeY *s, long i);
cell *mk_real(SchemeY *s, float r);
cell *mk_char(SchemeY *s, int ch);
cell *mk_string(SchemeY *s, char *str);
cell *mk_symbol(SchemeY *s, char *sym);
cell *mk_ffun(SchemeY *s, ffun_t *f);
cell *mk_vector(SchemeY *s, size_t sz);
cell *mk_table(SchemeY *s, size_t sz);
cell *mk_port(SchemeY *s, FILE *p);

vector *mk_vector_t(SchemeY *s, size_t sz);


/* Hash table methods */



#endif