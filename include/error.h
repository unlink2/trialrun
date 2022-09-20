#ifndef ERROR_H_
#define ERROR_H_

typedef enum Errors {
  OK,
  ERR_BUFFER,
  ERR_UNKNOWN,
  ERR_TRIAL_PARSER_UNEXPECTED_END,
  ERR_TRIAL_PARSER_VALUE_ERROR,
  ERR_TRIAL_PARSER_KEY_ERROR,
  ERR_FILE_READ_ERROR
} Errors;

char *error_to_str(Errors err);

#endif
