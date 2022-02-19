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
  Object *obj = obj_new(ObjInteger);
  obj->as.real = real;
  return obj;
}

Object *obj_string(char *str) {
  Object *obj = obj_new(ObjString);
  obj->as.string = str_new(str);
  return obj;
}

Object *obj_symbol(char *str) {
  Object *obj = obj_new(ObjSymbol);
  obj->as.string = str_new(str);
  return obj;
}

Object *obj_pair(Object *car, Object *cdr) {
  Object *obj = obj_new(ObjPair);
  obj->as.pair.data = car;
  obj->as.pair.next = cdr;
  return obj;
}

Object *obj_primitive(Object *(*primitive)(Object *)) {
  Object *obj = obj_new(ObjPrimitive);
  obj->as.primitive = primitive;
  return obj;
}

Object *obj_closure(Object *params, Object *body, Object *env) {
  Object *obj = obj_new(ObjClosure);
  obj->as.closure.params = params;
  obj->as.closure.body = body;
  obj->as.closure.env = env;
  return obj;
}

Object *obj_port(FILE *stream, char *mode) {
  Object *obj = obj_new(ObjPort);
  obj->as.port.stream = stream;
  obj->as.port.mode = mode;
  return obj;
}
