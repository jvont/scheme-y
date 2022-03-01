/*
** Subset of Scheme's standard procedures.
** reference: https://small.r7rs.org/attachment/r7rs.pdf
*/
#ifndef _SY_BUILTINS_H
#define _SY_BUILTINS_H

#include "object.h"

// TODO: arity checks

// void builtins_init(SyObj *env);

cell *syB_add(SchemeY *s, cell *args);
cell *syB_sub(SchemeY *s, cell *args);
cell *syB_mul(SchemeY *s, cell *args);
cell *syB_div(SchemeY *s, cell *args);

#endif