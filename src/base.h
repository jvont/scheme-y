/*
** Base library.
** reference: https://small.r7rs.org/attachment/r7rs.pdf
*/
#ifndef _SY_BUILTINS_H
#define _SY_BUILTINS_H

#include "object.h"

// void builtins_init(SyObj *env);

cell *eval(SchemeY *s, cell *expr, cell *env);
void print_expr(cell *expr);

#endif