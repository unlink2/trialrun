#include "trial.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>

#define BEGIN "===BEGIN"
#define END "===END"

typedef struct TrialParseResult {
  char *next;
  Errors err;
} TrialParseResult;

char *default_str_from(const char *d) {
  char *s = malloc(strlen(d) + 1);
  strncpy(s, d, strlen(d));
  return s;
}

// parse the next trial input and return the end pointer
TrialParseResult trial_parse_next(char *input) {
  TrialParseResult r;
  r.next = input;
  r.err = OK;

  return r;
}

Trial trial_from(char *input) {
  // end of input data pointer
  char *input_end = input + strlen(input);

  Trial trial;
  memset(&trial, 0, sizeof(Trial));

  // set up defaults
  trial.begin = default_str_from(BEGIN);
  trial.end = default_str_from(END);

  trial.ignore_lines = NULL;
  trial.ignore_lines_len = 0;

  trial.data_path = DEFAULT_PATH_OUT;
  trial.echo = FALSE;
  trial.expected_path = DEFAULT_PATH_IN;

  // parse until the end of input
  while (input < input_end) {
    TrialParseResult r = trial_parse_next(input);

    // propagate error
    if (r.err) {
      trial.err = r.err;
      return trial;
    }
    input = r.next;
  }

  return trial;
}

void trial_free(Trial *trial) {
  if (trial->err) {
    return;
  }
}
