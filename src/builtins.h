/*
** Subset of Scheme's standard procedures.
** reference: https://small.r7rs.org/attachment/r7rs.pdf
*/
#ifndef _SY_BUILTINS_H
#define _SY_BUILTINS_H

#include "object.h"

// TODO: arity checks

// void builtins_init(SyObj *env);

cell_t *syB_add(SchemeY *s, cell_t *args);
cell_t *syB_sub(SchemeY *s, cell_t *args);
cell_t *syB_mul(SchemeY *s, cell_t *args);
cell_t *syB_div(SchemeY *s, cell_t *args);

#endif