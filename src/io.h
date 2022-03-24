/*
** Scheme IO.
** Methods for reading from/writing to streams. Interactive prompt is handled
** internally by sy_read and related functions.
*/
#ifndef _IO_H
#define _IO_H

#include "object.h"

// char *token, *tp, *tend;  /* Token buffer */
// int lookahead;  /* Lookahead character */
// size_t depth, lineno;  /* Reader state */
// int prompt;  /* Interactive prompt mode? */

void print_obj(Object *obj);

// Object *sy_read_char(Object *port);
Object *sy_read(Object *port);
Object *sy_write(Object *args);

#endif