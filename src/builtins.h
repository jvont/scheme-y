#ifndef _BUILTINS_H
#define _BUILTINS_H

#include "object.h"
#include "runtime.h"

#include <stdio.h>

// void builtins_init(Env *e);

void print_expr(Object *expr);

Object *eval(Env *e);

#endif