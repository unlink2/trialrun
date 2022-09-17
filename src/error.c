#include "error.h"

char *error_to_str(Errors err) {
  switch (err) {
  case ERR_BUFFER:
    return "Buffer error";
  case OK:
    return "";
  default:
    return "Unknown error";
  }
}
