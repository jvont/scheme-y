/*
** Scheme IO.
** Methods for reading from/writing to streams. Interactive prompt is handled
** internally by sy_read and related functions.
*/
#ifndef _IO_H
#define _IO_H

#include "object.h"

void print_obj(cell *obj);

// cell *sy_read_char(SchemeY *s, cell *port);
cell *sy_read(SchemeY *s, cell *port);
cell *sy_write(SchemeY *s, cell *args);

#endif