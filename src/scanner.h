#ifndef _SCANNER_H
#define _SCANNER_H

#include "object.h"

#include <stdio.h>
#include <stdbool.h>

typedef struct Scanner {
  FILE *stream;  // input port, NULL if input
  char *input, *i;  // input buffer, NULL if stream
  char *buf;  // token buffer
  size_t p, size;  // token buffer position/size
  bool prompt;  // interactive prompt flag
  unsigned int depth;  // nested expression depth
  int ch;  // lookahead character
} Scanner;

Scanner *scanner_stream(FILE *stream);
Scanner *scanner_prompt();
void scanner_free(Scanner *s);

void next(Scanner *s);
Object *parse(Scanner *s);

#endif