#include "scanner.h"
#include "runtime.h"  // mem_

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifdef USE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

static int isdelim(int c) { return isspace(c) || c == '(' || c == ')' || c == ';'; }
static int isinitial(int c) { return isalpha(c) || strchr("!$%&*/:<=>?@^_~", c); }
static int issubseq(int c) { return isinitial(c) || isdigit(c) || strchr("+-.", c); }

static void save(Scanner *s, int c) {
  if (s->p >= s->size) {
    if (s->size > SIZE_MAX / 2) {
      fprintf(stderr, "maximum token buffer size reached\n");
      exit(1);
    }
    s->buf = (char *)mem_realloc(s->buf, 2 * s->size);
  }
  s->buf[s->p++] = c;
  s->buf[s->p] = '\0';
}

// Get the next character from input, else port if NULL.
void next(Scanner *s) {
  if (s->input)
    s->ch = (*s->i == '\0') ? EOF : *s->i++;
  else if (s->port)
    s->ch = fgetc(s->port->as.port.stream);
  else
    s->ch = EOF;  // ERROR: no input stream
}

static void save_next(Scanner *s) {
  save(s, s->ch);
  next(s);
}

static int match_next(Scanner *s, int c) {
  if (s->ch == c) {
    save_next(s);
    return 1;
  }
  return 0;
}

static int match_next2(Scanner *s, char *pair) {
  assert(pair[2] == '\0');
  if (s->ch == pair[0] || s->ch == pair[1]) {
    save_next(s);
    return 1;
  }
  return 0;
}

// Get user input, printing prompt to stdout.
void input(Scanner *s, char *prompt) {
#ifdef USE_READLINE
  free(s->input);
  s->input = s->i = readline(prompt);
  add_history(s->input);
  next(s);
#else
  printf("%s", prompt);
  fflush(stdout);
  next(s);
#endif
}

// exponent = [ + | - ] { digit }- space ;
static int scan_exponent(Scanner *s) {
  match_next2(s, "+-");
  if (!isdigit(s->ch))  // require digit
    return TokInvalid;
  while(isdigit(s->ch))
    save_next(s);
  return isdelim(s->ch) ? TokNumber : TokInvalid;
}

// suffix = { digit } ( exponent | space ) ;
static int scan_suffix(Scanner *s) {
  do {
    save_next(s);
    if (match_next2(s, "Ee"))
      return scan_exponent(s);
  } while(isdigit(s->ch));
  return isdelim(s->ch) ? TokNumber : TokInvalid;
}

// number = digits ( suffix | exponent | space ) ;
static int scan_number(Scanner *s) {
  int hasdot = (s->ch == '.');
  do {
    save_next(s);
    if (s->ch == '.' && !hasdot)
      return scan_suffix(s);
    else if (match_next2(s, "Ee"))
      return scan_exponent(s);
    // else if (match_next2(s, "+-"))
    //   return scan_complex(s);
    // else if (match_next(s, '/'))
    //   return scan_decimal(s);
  } while(isdigit(s->ch));
  return isdelim(s->ch) ? TokNumber : TokInvalid;
}

// identifier = initial { subseq } space ;
static int scan_identifier(Scanner *s) {
  do {
    save_next(s);
  } while(issubseq(s->ch));
  return isdelim(s->ch) ? TokIdentifier : TokInvalid;
}

// Create a new scanner, setting port/input to NULL.
Scanner *scanner_new() {
  Scanner *s = (Scanner *)malloc(sizeof(Scanner));
  if (s == NULL) exit(1);
  s->buf = (char *)malloc(BUFSIZ * sizeof(char));
  if (s->buf == NULL) exit(1);
  s->port = NULL;
  s->input = s->i = NULL;
  s->p = 0;
  s->size = BUFSIZ;
  s->ch = EOF;
  return s;
}

// Free scanner + token buffer.
void scanner_free(Scanner *s) {
  if (s != NULL)
    free(s->buf);
  free(s);
}

// Scan the next token.
void scan(Scanner *s) {
  s->p = 0;
  s->tok = TokInvalid;
  while (isspace(s->ch))  // skip whitespace
    next(s);
  if (isinitial(s->ch))  // identifier
    s->tok = scan_identifier(s);
  else if (isdigit(s->ch))  // number
    s->tok = scan_number(s);
  else switch (s->ch) {
    case EOF:
      s->tok = TokEOF;
      break;
    case '+': case '-':
      save_next(s);
      if (s->ch == '.')  // ( + | - ) . suffix
        s->tok = scan_suffix(s);
      else if (isdigit(s->ch))  // ( + | - ) number
        s->tok = scan_number(s);
      else if (isdelim(s->ch))  // ( + | - )
        s->tok = TokIdentifier;
      else
        s->tok = TokInvalid;
      break;
    case '.':
      save_next(s);
      if (s->ch == '.')  // .. subseq
        s->tok = scan_identifier(s);
      else if (isdigit(s->ch))  // . suffix
        s->tok = scan_suffix(s);
      else if (isdelim(s->ch))  // dotsep
        s->tok = TokDot;
      else
        s->tok = TokInvalid;
      break;
    case '"':
      next(s);
      while (s->ch != '"') {  // string
        if (s->ch == EOF)  // unterminated
          s->tok = TokInvalid;
        else
          save_next(s);
      }
      next(s);
      s->tok = TokString;
      break;
    case '#':
      save_next(s);
      if (match_next2(s, "tf"))  // boolean
        s->tok = TokBoolean;
      else switch (s->ch) {
        case '(':  // vector
          save_next(s);
          s->tok = TokVector;
          break;
        case '\\':  // character
          save_next(s);
          if (isdelim(s->ch)) {  // empty
            s->tok = TokCharacter;
            break;
          }
          // FALLTHROUGH
        default:
          s->tok = TokInvalid;
          break;
      }
      break;
    case '(':
      save_next(s);
      s->tok = TokLParen;
      break;
    case ')':
      save_next(s);
      s->tok = TokRParen;
      break;
    case '\'':
      save_next(s);
      s->tok = TokQuote;
      break;
    case '`':
      save_next(s);
      s->tok = TokQuasiQuote;
      break;
    case ',':
      save_next(s);
      if (match_next(s, '@'))  // ,@
        s->tok = TokCommaAt;
      else
        s->tok = TokComma;
      break;
    default:
      save_next(s);
      s->tok = TokInvalid;
      break;
  }
  if (s->tok == TokInvalid) {  // scan rest of invalid token
    while (!isdelim(s->ch))
      save_next(s);
  }
  s->buf[s->p] = '\0';
}
