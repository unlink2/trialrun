#ifndef TRIAL_H_
#define TRIAL_H_

#include "types.h"
#include "error.h"

/**
 * Struct representing a parsed
 * trial file
 */
typedef struct Trial {
  char *name;
  char *command;

  char *data_path;
  char *expected_path;

  // lines to ignore. array of line numbers
  u32 *ignore_lines;
  usize ignore_lines_len;

  u32 ignore_begin;
  u32 ignore_end;

  char *begin;
  char *end;

  bool echo;

  Errors err;
} Trial;

/**
 * Parse a .tr file from an input string
 * a tr file is simply a collection key=value pairs
 */
Trial trial_from(char *input);

void trial_free(Trial *trial);

#endif
