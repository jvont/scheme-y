/*
** Scheme-Y API and REPL entry point.
*/
#ifndef SCHEME_Y_H
#define SCHEME_Y_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// Interpreter state
typedef struct SyState SyState;

// Scheme object
typedef union Object Object;

// Foreign function
typedef int *(FFunction)(SyState *);

SyState *Sy_open();
void Sy_close(SyState *s);

void Sy_pushlist(SyState *s, Object *ca, Object *cd);
void Sy_pushinteger(SyState *s, int32_t i);
void Sy_pushreal(SyState *s, float r);
void Sy_pushcharacter(SyState *s, uint32_t ch);
void Sy_pushstring(SyState *s, const char *str);
void Sy_pushnstring(SyState *s, const char *str, size_t n);
void Sy_pushffunction(SyState *s, FFunction *f);
void Sy_pushvector(SyState *s, size_t size);
void Sy_pushport(SyState *s, FILE *p);

// void Sy_load();

#endif