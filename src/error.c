#include "error.h"

char *error_to_str(Errors err) {
  switch (err) {
  case ERR_BUFFER:
    return "Buffer error";
  case ERR_TRIAL_PARSER_KEY_ERROR:
    return "Key error";
  case ERR_TRIAL_PARSER_VALUE_ERROR:
    return "Value error";
  case ERR_TRIAL_PARSER_UNEXPECTED_END:
    return "Unexpected end";
  case ERR_FILE_OPEN:
    return "File open error";
  case OK:
    return "";
  case ERR_TRIAL_UNEXPECTED_DATA_END:
    return "Unexpected end of expected data";
  case ERR_PROCESS_UNEXPECTED_DATE_END:
    return "Unexpected end of command";
  case ERR_TRIAL_FAILURE:
    return "Trial failed!";
  default:
    return "Unknown error";
  }
}
