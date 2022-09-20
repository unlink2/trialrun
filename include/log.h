#ifndef LOG_H_
#define LOG_H_

#include <stdio.h>

typedef enum LogLevel { OUTPUT, ERROR, INFO, DEBUG, LOG_LEVEL_LEN } LogLevel;

int tr_fprintf(FILE *f, const LogLevel level, const char *format, ...);

#endif
