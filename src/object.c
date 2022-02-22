#include "object.h"
#include "runtime.h"

#include <stdlib.h>
#include <string.h>

static Object *obj_new(int kind) {
  Object *obj = (Object *)mem_calloc(1, sizeof(Object));
  obj->kind = kind;
  return obj;
}

static char *str_new(char *src) {
  char *dest = (char *)mem_calloc(strlen(src), sizeof(char));
  return strcpy(dest, src);
}

Object *obj_integer(long integer) {
  Object *obj = obj_new(ObjInteger);
  obj->as.integer = integer;
  return obj;
}

Object *obj_real(double real) {
  Object *obj = obj_new(ObjReal);
  obj->as.real = real;
  return obj;
}

Object *obj_boolean(bool boolean) {
  Object *obj = obj_new(ObjBoolean);
  obj->as.boolean = boolean;
  return obj;
}

// Object *obj_character(int character) {
// }

Object *obj_string(char *string) {
  Object *obj = obj_new(ObjString);
  obj->as.string = str_new(string);
  return obj;
}

Object *obj_symbol(char *symbol) {
  Object *obj = obj_new(ObjSymbol);
  obj->as.string = str_new(symbol);
  return obj;
}

Object *obj_cell(Object *car, Object *cdr) {
  Object *obj = obj_new(ObjPair);
  obj->as.cell.car = car;
  obj->as.cell.cdr = cdr;
  return obj;
}

Object *obj_primitive(Object *(*primitive)(Object *)) {
  Object *obj = obj_new(ObjPrimitive);
  obj->as.primitive = primitive;
  return obj;
}

Object *obj_closure(Object *params, Object *body) {
  Object *obj = obj_new(ObjClosure);
  obj->as.closure.params = params;
  obj->as.closure.body = body;
  return obj;
}

Object *obj_port(FILE *stream, char *mode) {
  Object *obj = obj_new(ObjPort);
  obj->as.port.stream = stream;
  obj->as.port.mode = mode;
  return obj;
}
