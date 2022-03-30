/*
** Scheme-Y API and entry point.
*/
#ifndef SCHEME_Y_H
#define SCHEME_Y_H

// Scheme interpreter state
typedef struct SyState SyState;

// Scheme object
typedef union Object Object;

// Scheme standard procedure
typedef Object *(Procedure)(Object *);

// Scheme special form (syntax)
typedef Object *(Syntax)(Object *, Object *);

SyState *Sy_open();
void Sy_close(SyState *s);

// void Sy_load();

#endif