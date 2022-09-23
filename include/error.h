#ifndef ERROR_H_
#define ERROR_H_

typedef enum Error {
  OK,
  ERR_BUFFER,
  ERR_UNKNOWN,
  ERR_TRIAL_PARSER_UNEXPECTED_END,
  ERR_TRIAL_PARSER_VALUE_ERROR,
  ERR_TRIAL_PARSER_KEY_ERROR,
  ERR_FILE_OPEN,
  ERR_TRIAL_UNEXPECTED_DATA_END,
  ERR_PROCESS_UNEXPECTED_DATE_END,
  ERR_TRIAL_FAILURE,
} Error;

char *error_to_str(Error err);

#endif
