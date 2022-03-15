/*
** Scheme IO.
** Methods for reading from/writing to streams. Interactive prompt is handled
** internally by sy_read and related functions.
*/
#ifndef _IO_H
#define _IO_H

#include "object.h"

void print_obj(cell_t *obj);

// cell_t *sy_read_char(SchemeY *s, cell_t *port);
cell_t *sy_read(SchemeY *s, cell_t *port);
cell_t *sy_write(SchemeY *s, cell_t *args);

#endif