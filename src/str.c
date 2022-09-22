#include "str.h"
#include <stdlib.h>
#include <string.h>

Str str_init(char *s, const usize len) { // NOLINT
  Str n = {len, s};

  return n;
}

bool str_eq(const Str left, const Str right) {
  if (left.len != right.len) {
    return FALSE;
  }

  return strncmp(left.raw, right.raw, left.len) == 0;
}

bool str_eq_raw(const Str left, char *right) {
  return str_eq(left, str_init(right, strlen(right)));
}

char *str_from(const char *d) {
  char *s = malloc(strlen(d) + 1);
  strncpy(s, d, strlen(d));
  return s;
}

char *str_to_str(const Str s) {
  char *n = malloc(s.len + 1);
  memset(n, 0, s.len + 1);
  strncpy(n, s.raw, s.len);

  return n;
}

bool str_starts_with_raw(const Str s, char *with) {
  usize len = strlen(with);
  if (s.len < len) {
    return FALSE;
  }

  // now we just init 2 strings with the lenght of with and
  // then compare
  Str s1 = str_init(s.raw, len);
  Str s2 = str_init(with, len);

  return str_eq(s1, s2);
}
