/*
** Scheme IO.
** Methods for reading from/writing to streams. Interactive prompt is handled
** internally by sy_read and related functions.
*/
#ifndef _IO_H
#define _IO_H

#include "object.h"

void print_obj(Cell *obj);

// Cell *sy_read_char(State *s, Cell *port);
Cell *sy_read(State *s, Cell *port);
Cell *sy_write(State *s, Cell *args);



void *parse_expr()

#endif