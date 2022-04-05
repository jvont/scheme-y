/*
** Expression parser.
*/
#ifndef PARSER_H
#define PARSER_H

#include "object.h"

#define BUFFER_SIZE 1024

enum {
  E_OK,
  E_EOF,
  E_TOKEN,
  E_DOTSEP,
  E_RANGE,
  E_PARSE
};

int Parser_parse(SyState *s);

#endif