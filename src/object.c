#include "object.h"
#include "runtime.h"

#include <stdlib.h>
#include <string.h>

static Obj *obj_new(int kind) {
  Obj *obj = (Obj *)mem_calloc(1, sizeof(Obj));
  obj->kind = kind;
  return obj;
}

static char *str_new(char *src) {
  char *dest = (char *)mem_calloc(strlen(src), sizeof(char));
  return strcpy(dest, src);
}

Obj *obj_integer(long integer) {
  Obj *obj = obj_new(ObjInteger);
  obj->as.integer = integer;
  return obj;
}

Obj *obj_real(double real) {
  Obj *obj = obj_new(ObjReal);
  obj->as.real = real;
  return obj;
}

Obj *obj_boolean(bool boolean) {
  Obj *obj = obj_new(ObjBoolean);
  obj->as.boolean = boolean;
  return obj;
}

Obj *obj_character(int character) {
  Obj *obj = obj_new(ObjCharacter);
  obj->as.character = character;
  return obj;
}

Obj *obj_string(char *string) {
  Obj *obj = obj_new(ObjString);
  obj->as.string = str_new(string);
  return obj;
}

Obj *obj_symbol(char *symbol) {
  Obj *obj = obj_new(ObjSymbol);
  obj->as.string = str_new(symbol);
  return obj;
}

Obj *obj_primitive(Obj *(*primitive)(Obj *)) {
  Obj *obj = obj_new(ObjPrimitive);
  obj->as.primitive = primitive;
  return obj;
}

Obj *obj_procedure(Proc* procedure) {
  Obj *obj = obj_new(ObjProcedure);
  obj->as.procedure = procedure;
  return obj;
}

Obj *obj_cell(Obj *car, Obj *cdr) {
  Obj *obj = obj_new(ObjCell);
  obj->as.cell.car = car;
  obj->as.cell.cdr = cdr;
  return obj;
}

Obj *obj_closure(Obj *params, Obj *body) {
  Obj *obj = obj_new(ObjClosure);
  obj->as.closure.params = params;
  obj->as.closure.body = body;
  return obj;
}

Obj *obj_port(FILE *stream, char *mode) {
  Obj *obj = obj_new(ObjPort);
  obj->as.port.stream = stream;
  strncpy(obj->as.port.mode, mode, 2);
  return obj;
}

Obj *obj_vector(size_t size) {
  Obj *obj = obj_new(ObjVector);
  obj->as.vector.items = mem_calloc(size, sizeof(Obj));
  obj->as.vector.size = size;
  return obj;
}

void obj_free(Obj *obj) {
  switch (obj->kind) {
    case ObjString:
    case ObjSymbol:
      free(obj->as.string);
      break;
    case ObjCell:
      obj_free(obj->as.cell.car);
      obj_free(obj->as.cell.cdr);
      break;
    case ObjClosure:
      obj_free(obj->as.closure.params);
      obj_free(obj->as.closure.body);
      break;
    case ObjVector:
      free(obj->as.vector.items);
    default:
      break;
  }
  free(obj);
}
