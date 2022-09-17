#ifndef LOG_H_
#define LOG_H_

typedef enum LogLevel { OUTPUT, DEBUG, INFO, ERROR, LOG_LEVEL_LEN } LogLevel;

int tr_printf(const LogLevel level, const char *format, ...);

#endif
