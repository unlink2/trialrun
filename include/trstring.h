#ifndef TRSTRING_H_
#define TRSTRING_H_

#include "types.h"

typedef struct TrStr {
  usize len;
  char *raw;
} TrStr;

// Create an unmanaged tr string from an existing pointer
// You supply the data!
TrStr trstr_init(char *s, const usize len);

// Compare a trstr for equality
bool trstr_eq(const TrStr left, const TrStr right);

bool trstr_eq_raw(const TrStr left, char *right);

// create an owned C string copy from the input string
char *str_from(const char *d);

// convert a trstr to a malloced c-string
char *trstr_to_str(const TrStr s);

bool trstr_starts_with_raw(const TrStr s, char *with);

#endif
