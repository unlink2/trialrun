#ifndef LOG_H_
#define LOG_H_

typedef enum LogLevel { OUTPUT, INFO, ERROR, DEBUG, LOG_LEVEL_LEN } LogLevel;

int tr_printf(const LogLevel level, const char *format, ...);

#endif
