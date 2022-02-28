#include "object.h"
#include "state.h"
#include "mem.h"

#include <stdlib.h>
#include <string.h>

cell *syO_integer(SchemeY *s, long integer) {
  cell *p = syM_alloc(s, sizeof(cell));
  p->kind = TyInteger;
  p->as.integer = integer;
  return p;
}

cell *syO_real(SchemeY *s, double real) {
  cell *p = syM_alloc(s, sizeof(cell));
  p->kind = TyReal;
  p->as.real = real;
  return p;
}

cell *syO_character(SchemeY *s, int character) {
  cell *p = syM_alloc(s, sizeof(cell));
  p->kind = TyCharacter;
  p->as.character = character;
  return p;
}

cell *syO_string(SchemeY *s, char *string) {
  cell *p = syM_alloc(s, sizeof(cell));
  p->kind = TyString;
  p->as.string = syM_strdup(s, string);
  return p;
}

cell *syO_symbol(SchemeY *s, char *symbol) {
  cell *p = syM_alloc(s, sizeof(cell));
  p->kind = TySymbol;
  p->as.string = syM_strdup(s, symbol);
  return p;
}

cell *syO_ffun(SchemeY *s, ffun *ffun) {
  cell *p = syM_alloc(s, sizeof(cell));
  p->kind = TyFFun;
  p->as.ffun = ffun;
  return p;
}

cell *syO_cons(SchemeY *s, cell *car, cell *cdr) {
  cell *p = syM_alloc(s, sizeof(cell));
  p->kind = TyPair;
  p->as.pair.car = car;
  p->as.pair.cdr = cdr;
  return p;
}

cell *syO_vector(SchemeY *s, unsigned int size) {
  cell *arr = syM_calloc(s, size + 1, sizeof(cell));
  cell *p = arr;
  p->kind = TyVector;
  p->as.vector.items = arr + 1;
  p->as.vector.length = 0;
  p->as.vector.size = size;
  return p;
}

cell *syO_table(SchemeY *s, unsigned int size) {
  cell *arr = syM_calloc(s, size + 1, sizeof(cell));
  cell *p = arr;
  p->kind = TyTable;
  p->as.vector.items = arr + 1;
  p->as.vector.length = 0;
  p->as.vector.size = size;
  for (unsigned int i = 1; i < size + 1; i++)
    arr[i].kind = TyPair;
  return p;
}

cell *syO_port(SchemeY *s, FILE *stream, char *mode) {
  cell *p = syM_alloc(s, sizeof(cell));
  p->kind = TyPort;
  p->as.port.stream = stream;
  strncpy(p->as.port.mode, mode, 2);
  return p;
}
