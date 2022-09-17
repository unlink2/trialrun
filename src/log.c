#include "log.h"
#include <stdarg.h>
#include <stdio.h>
#include "config.h"

int tr_printf(const LogLevel level, const char *format, ...) {
  cfg();
  va_list argptr;
  va_start(argptr, format);

  if (level > cfg->log_level) {
    return 0;
  }
  int len = vprintf(format, argptr);

  va_end(argptr);

  return len;
}
