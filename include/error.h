#ifndef ERROR_H_
#define ERROR_H_

typedef enum Errors { OK, ERR_BUFFER, ERR_UNKNOWN } Errors;

char *error_to_str(Errors err);

#endif
