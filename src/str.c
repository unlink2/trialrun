#include "str.h"
#include <stdlib.h>
#include <string.h>

void str_print(FILE *f, const Str s) {
  for (int i = 0; i < s.len; i++) {
    fputc(s.raw[i], f);
  }
}
