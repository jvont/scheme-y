#ifndef _OBJECT_H
#define _OBJECT_H

#include <stdio.h>
#include <stdbool.h>

struct Env;

typedef struct Object {
  enum {
    ObjInteger, ObjReal, ObjBoolean, ObjCharacter,
    ObjString, ObjSymbol, ObjPair, ObjPrimitive,
    ObjProcedure, ObjClosure, ObjPort
    // ObjRational, ObjComplex,
    // ObjTable, ObjVector,
  } kind;
  int marked;  // gc tag
  union {
    long integer;
    double real;
    // struct { long n; long d; } rational;
    // struct { double r; double i; } complex;
    bool boolean;
    int character;
    char *string;
    struct { struct Object *car, *cdr; } cell;
    struct Object *(*primitive)(struct Object *);
    struct Object *(*procedure)(struct Object *, struct Env *);
    struct { struct Object *params, *body; } closure;
    struct { FILE *stream; char *mode; } port;
    // struct { struct Object *items; size_t size; } vector;
  } as;
} Object;

Object *obj_integer(long integer);
Object *obj_real(double real);
Object *obj_boolean(bool boolean);
// Object *obj_character(int character);
Object *obj_string(char *string);
Object *obj_symbol(char *symbol);
Object *obj_cell(Object *car, Object *cdr);
Object *obj_primitive(Object *(*primitive)(Object *));
Object *obj_closure(Object *params, Object *body);
Object *obj_port(FILE *stream, char *mode);

// #define TABLE_CAPACITY 32
// Object *obj_array(size_t size);

// size_t length(Object *list);

#endif