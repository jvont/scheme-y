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

#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

typedef struct State State;

typedef union Object Object;
typedef struct Vector Vector;

typedef Object *(Procedure)(Object *);
typedef Object *(Syntax)(Object *, Object *);

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
    T_PORT,
    T_FWD
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
    Object *fwd;
  } as;
} Atom;

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

#define isinteger(x)   (isatom(x) && type(x) == T_INTEGER)
#define isreal(x)      (isatom(x) && type(x) == T_REAL)
#define isnumber(x)    (isatom(x) && (isinteger(x) || isreal(x)))
#define ischaracter(x) (isatom(x) && type(x) == T_CHARACTER)
#define isstring(x)    (isatom(x) && type(x) == T_STRING)
#define issymbol(x)    (isatom(x) && type(x) == T_SYMBOL)
#define isprocedure(x) (isatom(x) && type(x) == T_PROCEDURE)
#define issyntax(x)    (isatom(x) && type(x) == T_SYNTAX)
#define isvector(x)    (isatom(x) && type(x) == T_VECTOR)
#define istable(x)     (isatom(x) && type(x) == T_TABLE)
#define isport(x)      (isatom(x) && type(x) == T_PORT)
#define isfwd(x)       (isatom(x) && type(x) == T_FWD)

#define set_int(x,i)    (type(x) = T_INTEGER, as(x).integer = i)
#define set_real(x,r)   (type(x) = T_REAL, as(x).real = r)

// Object *mk_cons(Object *, Object *d);
// Object *mk_integer(long i);
// Object *mk_real(float r);
// Object *mk_character(int c);
// Object *mk_string(const char *s);
// Object *mk_symbol(const char *s);
// Object *mk_procedure(Procedure *p);
// Object *mk_syntax(Syntax *s);
// Object *mk_vector(size_t n);
// Object *mk_table(size_t n);
// Object *mk_port(FILE *p);

#endif