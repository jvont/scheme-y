#ifndef _SCANNER_H
#define _SCANNER_H

#include "object.h"

typedef struct Scanner {
  Object *port;  // input port
  char *buf;  // token buffer
  size_t p, size;  // buffer position/size
  char *input, *i;  // input buffer or NULL if port
  int ch;  // lookahead character
  enum {  // current token
    TokInvalid, TokEOF, TokDot, TokLParen, TokRParen,
    TokQuote, TokQuasiQuote, TokComma, TokCommaAt, TokVector,
    TokIdentifier, TokBoolean, TokNumber, TokCharacter, TokString
  } tok;
} Scanner;

Scanner *scanner_new();
void scanner_free(Scanner *s);

void input(Scanner *s, char *prompt);
void next(Scanner *s);
void scan(Scanner *s);

#endif