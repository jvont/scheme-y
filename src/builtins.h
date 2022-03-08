/*
** Subset of Scheme's standard procedures.
** reference: https://small.r7rs.org/attachment/r7rs.pdf
*/
#ifndef _SY_BUILTINS_H
#define _SY_BUILTINS_H

#include "object.h"

// Interface to specify argument range (min/max) and
// test types of sub-ranges of arguments

// void builtins_init(SyObj *env);

cell_t *sy_add(SchemeY *s, cell_t *args);
cell_t *sy_sub(SchemeY *s, cell_t *args);
cell_t *sy_mul(SchemeY *s, cell_t *args);
cell_t *sy_div(SchemeY *s, cell_t *args);

#endif