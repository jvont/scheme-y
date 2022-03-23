/*
** Subset of Scheme's standard procedures.
** reference: https://small.r7rs.org/attachment/r7rs.pdf
*/
#ifndef _BUILTINS_H
#define _BUILTINS_H

#include "object.h"

// Interface to specify argument range (min/max) and
// test types of sub-ranges of arguments

// void builtins_init(SyObj *env);

Cell *sy_add(State *s, Cell *args);
Cell *sy_sub(State *s, Cell *args);
Cell *sy_mul(State *s, Cell *args);
Cell *sy_div(State *s, Cell *args);

#endif