#ifndef _OBJECT_H
#define _OBJECT_H

#include <stdio.h>
#include <stdbool.h>

struct Obj;
struct Env;

typedef struct Obj *(Func)(struct Obj *);
typedef struct Obj *(Proc)(struct Obj *, struct Env *);

typedef struct Obj {
  enum {
    ObjInteger, ObjReal, // ObjRational, ObjComplex,
    ObjBoolean, ObjCharacter, ObjString, ObjSymbol,
    ObjPrimitive, ObjProcedure, ObjCell, ObjClosure,
    ObjPort, ObjTable, ObjVector,
  } kind;
  bool marked;  // gc tag
  union {
    long integer;
    double real;
    // struct { long n; long d; } rational;
    // struct { double r; double i; } complex;
    bool boolean;
    int character;
    char *string;
    Func *primitive;
    Proc *procedure;
    struct { struct Obj *car, *cdr; } cell;
    struct { struct Obj *params, *body; } closure;
    struct { FILE *stream; char mode[2]; } port;
    struct { struct Obj *items; size_t size; } vector;
    struct Env* environment;
  } as;
} Obj;

typedef struct Env {
  Obj *bindings;
  struct Env *parent;
} Env;

Obj *obj_integer(long integer);
Obj *obj_real(double real);
Obj *obj_boolean(bool boolean);
Obj *obj_character(int character);
Obj *obj_string(char *string);
Obj *obj_symbol(char *symbol);
Obj *obj_primitive(Func* primitive);
Obj *obj_procedure(Proc* procedure);
Obj *obj_cell(Obj *car, Obj *cdr);
Obj *obj_closure(Obj *params, Obj *body);
Obj *obj_port(FILE *stream, char *mode);
Obj *obj_vector(size_t size);

void obj_free(Obj *obj);

#endif