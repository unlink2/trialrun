#include "trstring.h"
#include <stdlib.h>
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

bool trstr_eq_raw(const TrStr left, const char *right) {
  return trstr_eq(left, trstr_init(right, strlen(right)));
}

char *str_from(const char *d) {
  char *s = malloc(strlen(d) + 1);
  strncpy(s, d, strlen(d));
  return s;
}

char *trstr_to_str(const TrStr s) {
  char *n = malloc(s.len + 1);
  memset(n, 0, s.len + 1);
  strncpy(n, s.raw, s.len);

  return n;
}
