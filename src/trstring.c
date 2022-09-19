#include "trstring.h"
#include <string.h>

TrStr trstr_init(const char *s, const usize len) {
  TrStr n = {len, s};

  return n;
}

bool trstr_eq(const TrStr left, const TrStr right) {
  if (left.len != right.len) {
    return FALSE;
  }

  return strncmp(left.raw, right.raw, left.len) == 0;
}
