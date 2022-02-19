#ifndef _OBJECT_H
#define _OBJECT_H

#include <stdio.h>

typedef struct Object {
  enum {
    ObjInteger, ObjReal, ObjCharacter, ObjString,
    ObjSymbol, ObjBoolean, ObjPair, ObjPrimitive,
    ObjProcedure, ObjClosure, ObjPort
    // ObjRational, ObjComplex, ObjTable
  } kind;
  union {
    long integer;
    double real;
    // struct { long n; long d; } rational;
    // struct { double r; double i; } complex;
    int character;
    char *string;
    struct { struct Object *data, *next; } pair;
    struct Object *(*primitive)(struct Object *);
    struct Object *(*procedure)(struct Object *, struct Env *);
    struct { struct Object *params, *body, *env; } closure;
    struct { FILE *stream; char *mode; } port;
    // struct { struct Object *items; size_t size; } table;
  } as;
} Object;

Object *obj_integer(long integer);
Object *obj_real(double real);
Object *obj_string(char *str);
Object *obj_symbol(char *str);
Object *obj_pair(Object *car, Object *cdr);
Object *obj_primitive(Object *(*primitive)(Object *));
Object *obj_closure(Object *params, Object *body, Object *env);
Object *obj_port(FILE *stream, char *mode);

// size_t length(Object *list);

#endif