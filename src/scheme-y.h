/*
** Scheme-Y API and REPL entry point.
*/
#ifndef SCHEME_Y_H
#define SCHEME_Y_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// Default read buffer size
#define BUFFER_SIZE 1024

// Interpreter state
typedef struct SyState SyState;

// Scheme object
typedef struct Object Object;

// Foreign C function
typedef int *(FFunction)(SyState *);

// Create new interpreter state
SyState *sy_open();
void sy_close(SyState *s);

// C Stack API
void sy_pushlist(SyState *s, Object *ca, Object *cd);
void sy_pushnil(SyState *s);
void sy_pushinteger(SyState *s, int32_t i);
void sy_pushreal(SyState *s, float r);
void sy_pushboolean(SyState *s, int b);
void sy_pushcharacter(SyState *s, uint32_t ch);
void sy_pushstring(SyState *s, const char *str);
void sy_pushnstring(SyState *s, const char *str, size_t n);
void sy_pushffunction(SyState *s, FFunction *f);
void sy_pushvector(SyState *s, size_t size);
void sy_pushport(SyState *s, FILE *p);

// void sy_error(SyState *s, const char *msg);

void sy_intern(SyState *s, const char *sym);
void sy_nintern(SyState *s, const char *sym, size_t n);

// TEMP
extern int read_read(SyState *s);


#endif