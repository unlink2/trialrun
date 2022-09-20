#include "log.h"
#include <stdarg.h>
#include <stdio.h>
#include "config.h"

int tr_fprintf(FILE *f, const LogLevel level, const char *format, ...) {
  Config *cfg = cfg();
  va_list argptr;
  va_start(argptr, format);

  if (level > cfg->log_level) {
    return 0;
  }
  int len = vfprintf(f, format, argptr);

  va_end(argptr);

  return len;
}
