#include "trial.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "macros.h"
#include "trstring.h"

#define BEGIN "===BEGIN"
#define END "===END"
#define DELIM '='

typedef struct TrialParseResult {
  char *start;
  char *next;
  Errors err;
} TrialParseResult;

char *default_str_from(const char *d) {
  char *s = malloc(strlen(d) + 1);
  strncpy(s, d, strlen(d));
  return s;
}

bool trial_is_end(char c) { return c == '\0' || c == '\n' || c == '\r'; }

bool trial_bool_val(TrStr value, Errors *err) {
  *err = OK;
  if (trstr_eq_raw(value, "true", 4)) {
    return TRUE;
  } else if (trstr_eq_raw(value, "false", 5)) {
    return FALSE;
  } else {
    *err = ERR_TRIAL_PARSER_VALUE_ERROR;
    return FALSE;
  }
}

Errors trial_parse_handle(Trial *t, TrStr key, TrStr value) {

  Errors err = OK;

  // dumb check for each possible key
  // TODO there has to be a better way!
  if (trstr_eq_raw(key, "echo", 4)) {
    t->echo = trial_bool_val(value, &err);
  } else {
    return ERR_TRIAL_PARSER_KEY_ERROR;
  }

  return err;
}

// parse the next trial input and return the end pointer
TrialParseResult trial_parse_next(Trial *t, char *input) {
  TrialParseResult r;
  r.start = input; // start of line

  TrStr key = trstr_init(input, 0);

  // get key until = or end of line
  while (input[0] != DELIM && !trial_is_end(input[0])) {
    input++;
    key.len++;
  }

  // if we did not find an = the file is invalid
  if (input[0] == '\0') {
    r.next = input; // the char where the invalid state occured
    r.err = ERR_TRIAL_PARSER_UNEXPECTED_END;
    return r;
  }

  // otherwise we now have a key
  TrStr value = trstr_init(++input, 0); // char after = is start of value

  // obtain value
  while (!trial_is_end(input[0])) {
    input++;
    value.len++;
  }

  r.next = input;
  r.err = trial_parse_handle(t, key, value);

  return r;
}

void trial_init(Trial *t) {
  memset(t, 0, sizeof(Trial));

  // set up defaults
  t->begin = default_str_from(BEGIN);
  t->end = default_str_from(END);
  t->test_line_prefix = default_str_from("");

  t->data_path = DEFAULT_PATH_OUT;
  t->echo = FALSE;
  t->expected_path = DEFAULT_PATH_IN;
}

Trial trial_from(char *input) {
  // end of input data pointer
  char *input_end = input + strlen(input);

  Trial trial;
  trial_init(&trial);

  // parse until the end of input
  while (input < input_end) {
    TrialParseResult r = trial_parse_next(&trial, input);

    // propagate error
    if (r.err) {
      trial.err = r.err;
      return trial;
    }
    input = r.next;
  }

  return trial;
}

void trial_run(Trial *t, FILE *out) {
  if (t->err) {
    return;
  }
}

void trial_free(Trial *trial) {
  if (trial->err) {
    return;
  }

  free(trial->begin);
  free(trial->end);
  free(trial->test_line_prefix);
}

#ifdef TEST

#include "macros.h"

void test_trial_parse_next(void **state) {
  {
    Trial t;
    trial_init(&t);
    assert_false(t.echo);
    TrialParseResult r = trial_parse_next(&t, "echo=true");

    assert_true(r.err == OK);
    assert_true(t.echo);
    trial_free(&t);
  }
  {
    Trial t;
    trial_init(&t);
    TrialParseResult r = trial_parse_next(&t, "ech=true");

    assert_true(r.err == ERR_TRIAL_PARSER_KEY_ERROR);
    trial_free(&t);
  }
  {
    Trial t;
    trial_init(&t);
    TrialParseResult r = trial_parse_next(&t, "echo=tru");

    assert_true(r.err == ERR_TRIAL_PARSER_VALUE_ERROR);
    trial_free(&t);
  }
}

#endif
