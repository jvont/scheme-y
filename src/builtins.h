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

cell *sy_add(SchemeY *s, cell *args);
cell *sy_sub(SchemeY *s, cell *args);
cell *sy_mul(SchemeY *s, cell *args);
cell *sy_div(SchemeY *s, cell *args);

#endif