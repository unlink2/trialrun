#ifndef TRSTRING_H_
#define TRSTRING_H_

#include "types.h"

typedef struct TrStr {
  usize len;
  const char *raw;
} TrStr;

// Create an unmanaged tr string from an existing pointer
// You supply the data!
TrStr trstr_init(const char *s, const usize len);

// Compare a trstr for equality
bool trstr_eq(const TrStr left, const TrStr right);

bool trstr_eq_raw(const TrStr left, const char *right, usize len);

#endif
