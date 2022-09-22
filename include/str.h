#ifndef STR_H_
#define STR_H_

#include "types.h"

typedef struct Str {
  usize len;
  char *raw;
} Str;

// Create an unmanaged tr string from an existing pointer
// You supply the data!
Str str_init(char *s, const usize len);

// Compare a str for equality
bool str_eq(const Str left, const Str right);

bool str_eq_raw(const Str left, char *right);

// create an owned C string copy from the input string
char *str_from(const char *d);

// convert a str to a malloced c-string
char *str_to_str(const Str s);

bool str_starts_with_raw(const Str s, char *with);

#endif
