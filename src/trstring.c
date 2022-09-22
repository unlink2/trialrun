#include "trstring.h"
#include <stdlib.h>
#include <string.h>

TrStr trstr_init(char *s, const usize len) { // NOLINT
  TrStr n = {len, s};

  return n;
}

bool trstr_eq(const TrStr left, const TrStr right) {
  if (left.len != right.len) {
    return FALSE;
  }

  return strncmp(left.raw, right.raw, left.len) == 0;
}

bool trstr_eq_raw(const TrStr left, char *right) {
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

bool trstr_starts_with_raw(const TrStr s, char *with) {
  usize len = strlen(with);
  if (s.len < len) {
    return FALSE;
  }

  // now we just init 2 trstrings with the lenght of with and
  // then compare
  TrStr s1 = trstr_init(s.raw, len);
  TrStr s2 = trstr_init(with, len);

  return trstr_eq(s1, s2);
}
