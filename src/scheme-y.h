#ifndef _SCHEME_Y_H
#define _SCHEME_Y_H

#include "object.h"

#include <stdio.h>
#include <stdbool.h>

typedef enum ErrorCode {
  ErrOK, ErrEOF, ErrInvalid, ErrDotsep
} ErrorCode;

typedef struct SchemeY {
  FILE *stream;  // input stream
  char *line, *i;  // input line buffer
  int ch;  // lookahead character
  char *buf;  // token buffer
  size_t p, size;  // token buffer position/size
  size_t lineno;  // current line number
  size_t depth;  // nested expression depth
  ErrorCode err;  // error status
} SchemeY;

SchemeY *sy_new(FILE *stream);
void sy_free(SchemeY *s);

Obj *sy_parse(SchemeY *s);
void sy_error(SchemeY *s);

#endif