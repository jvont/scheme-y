/*
** Reader/parser state. 
** Parses input streams into Scheme objects, storing symbols.
*/
#ifndef _SY_READ_H
#define _SY_READ_H

#include "object.h"

#include <stdio.h>
#include <stdlib.h>

// TODO: parse character literals + reserved symbols.
// TODO: add read objects to the stack to prevent gc.

typedef struct SyReader {
  FILE *stream;  // input stream
  int ch;  // lookahead character
  char buf[BUFSIZ];  // token buffer
  size_t bp;  // token buffer position
  size_t lineno;  // current line number
  size_t depth;  // nested expression depth
  enum {  // read status
    ErrOK, ErrEOF, ErrInvalid, ErrDotsep
  } err;
  SchemeY *s;
} SyReader;

cell *syR_read(SchemeY *s, cell *arg);

#endif