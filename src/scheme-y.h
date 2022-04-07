/*
** Scheme-y C API.
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

SyState *sy_open();
void sy_close(SyState *s);

// C function
typedef int *(SyFunction)(SyState *);

// C Stack API
void sy_pushlist(SyState *s);
void sy_pushnil(SyState *s);
void sy_pushinteger(SyState *s, int32_t i);
void sy_pushreal(SyState *s, float r);
void sy_pushboolean(SyState *s, int b);
void sy_pushcharacter(SyState *s, uint32_t ch);
void sy_pushstring(SyState *s, const char *str);
void sy_pushnstring(SyState *s, const char *str, size_t n);
void sy_pushffunction(SyState *s, SyFunction *f);
void sy_pushvector(SyState *s, size_t size);
void sy_pushport(SyState *s, FILE *p);

// void sy_error(SyState *s, const char *msg);

void sy_intern(SyState *s, const char *sym);
void sy_nintern(SyState *s, const char *sym, size_t n);

// TEMP
extern int read_read(SyState *s);


#endif