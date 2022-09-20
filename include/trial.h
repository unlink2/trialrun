#ifndef TRIAL_H_
#define TRIAL_H_

#include "types.h"
#include "error.h"
#include <stdio.h>

/**
 * Struct representing a parsed
 * trial file
 */
typedef struct Trial {
  char *name;
  char *command;

  char *data_path;
  char *expected_path;

  char *test_line_prefix;

  char *begin;
  char *end;

  bool echo;

  Errors err;
} Trial;

void trial_init(Trial *t);

/**
 * Parse a .tr file from an input string
 * a tr file is simply a collection key=value pairs
 */
Trial trial_from(char *input);

void trial_print(Trial *t, FILE *f);

void trial_run(Trial *t, FILE *out);

void trial_free(Trial *trial);

#ifdef TEST

void test_trial_parse_next(void **state);

#endif

#endif
