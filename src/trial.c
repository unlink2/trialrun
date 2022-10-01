#include "trial.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "macros.h"
#include "str.h"
#include <scl.h>

#define BEGIN "===BEGIN"
#define END "===END"
#define DELIM '='

typedef struct TrialParseResult {
  char *start;
  char *next;
  Error err;
} TrialParseResult;

void trial_state_init(TrialState *s) { memset(s, 0, sizeof(TrialState)); }

bool trial_is_end(char c) { return c == '\0' || c == '\n' || c == '\r'; }

bool trial_bool_val(Str value, Error *err) {
  *err = OK;
  if (str_eq_raw(value, "true")) {
    return TRUE;
  } else if (str_eq_raw(value, "false")) {
    return FALSE;
  } else {
    char *failed_value = str_to_str(value);
    scl_log_fprintf(stderr, ERROR, "Value Error: %s %d\n", failed_value,
                    value.len);
    free(failed_value);
    *err = ERR_TRIAL_PARSER_VALUE_ERROR;
    return FALSE;
  }
}

Error trial_parse_handle(Trial *t, Str key, Str value) {
  Error err = OK;

  // dumb check for each possible key
  // TODO there has to be a better way!
  if (str_eq_raw(key, "echo")) {
    t->echo = trial_bool_val(value, &err);
  } else if (str_eq_raw(key, "name")) {
    free(t->name);
    t->name = str_to_str(value);
  } else if (str_eq_raw(key, "command")) {
    free(t->command);
    t->command = str_to_str(value);
  } else if (str_eq_raw(key, "data")) {
    free(t->data_path);
    t->data_path = str_to_str(value);
  } else if (str_eq_raw(key, "expected")) {
    free(t->expected_path);
    t->expected_path = str_to_str(value);
  } else if (str_eq_raw(key, "test-line-prefix")) {
    free(t->test_line_prefix);
    t->test_line_prefix = str_to_str(value);
  } else if (str_eq_raw(key, "begin")) {
    free(t->begin);
    t->begin = str_to_str(value);
  } else if (str_eq_raw(key, "end")) {
    free(t->end);
    t->end = str_to_str(value);
  } else {
    char *failed_key = str_to_str(key);
    scl_log_fprintf(stderr, ERROR, "Key error '%s'\n", failed_key);
    free(failed_key);
    return ERR_TRIAL_PARSER_KEY_ERROR;
  }

  return err;
}

char *trial_skip_whitespace(char *input) {
  // skip leading whitespaces
  while (isspace(input[0])) {
    input++;
  }

  return input;
}

char *trial_skip_comment(char *input) {
  input = trial_skip_whitespace(input);
  while (input[0] == '#') {
    // loop until next line
    while (input[0] != '\n' && !trial_is_end(input[0])) {
      input++;
    }
    input = trial_skip_whitespace(input);
  }
  return input;
}

// parse the next trial input and return the end pointer
TrialParseResult trial_parse_next(Trial *t, char *input) {
  TrialParseResult r;

  input = trial_skip_whitespace(input);
  input = trial_skip_comment(input);

  if (input[0] == '\0') {
    r.next = input;
    r.err = OK;
    return r;
  }

  r.start = input; // start of line

  Str key = str_init(input, 0);

  // get key until = or end of line
  while (input[0] != DELIM && !trial_is_end(input[0])) {
    input++;
    key.len++;
  }

  // if we did not find an = the file is invalid
  if (input[0] == '\0') {
    scl_log_fprintf(stderr, ERROR, "Unexpected end: %s\n", input);
    r.next = input; // the char where the invalid state occured
    r.err = ERR_TRIAL_PARSER_UNEXPECTED_END;
    return r;
  }

  // otherwise we now have a key
  Str value = str_init(++input, 0); // char after = is start of value

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
  t->begin = str_from(BEGIN);
  t->end = str_from(END);
  t->test_line_prefix = str_from("");

  t->data_path = str_from(DEFAULT_PATH_OUT);
  t->echo = FALSE;
  t->expected_path = str_from(DEFAULT_PATH_IN);
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

int trial_read_line_from(FILE *f, FILE *out, StrBuffer *buffer, bool echo) {
  strbuf_clear(buffer);
  int b = 0;
  // read process output into buffer line by line
  // and compare lines that match the comparison criteria
  while ((b = fgetc(f)) != EOF) {
    if (echo) {
      putc(b, out);
    }

    // this will also catch \r\n
    if (b == '\n') {
      break;
    }

    strbuf_write(buffer, (char)b);
  }

  // return last byte, if it is EOF the caller will know
  return b;
}

TrialState trial_run(Trial *t, FILE *out) {
  Config *cfg = cfg();
  TrialState state;
  trial_state_init(&state);
  if (t->err) {
    return state;
  }

  trial_print(t, out);

  state.success = TRUE;

  scl_log_fprintf(out, OUTPUT, "[%s] Running '%s'\n", t->name, t->command);

  FILE *pio = popen(t->command, "re"); // NOLINT
  FILE *eio = fopen(t->expected_path, "re");

  if (!pio || !eio) {
    scl_log_fprintf(stderr, ERROR, "[%s] Unable to open expected file '%s'!\n",
                    t->name, t->expected_path);
    state.err = ERR_FILE_OPEN;
    return state;
  }

  StrBuffer input = strbuf_init(32);
  StrBuffer expected = strbuf_init(32);

  usize line = 1;

  while (trial_read_line_from(pio, out, &input,
                              t->echo || cfg->log_level >= LOG_LEVEL_LEN) !=
         EOF) {
    if (!str_starts_with_raw(input.str, t->test_line_prefix)) {
      continue;
    }

    // if there is no more test data it is an automatic failure
    if (trial_read_line_from(eio, out, &expected, FALSE) == EOF) {
      state.err = ERR_TRIAL_UNEXPECTED_DATA_END;
      break;
    }

    // otherwise we can compare!
    if (!str_eq(input.str, expected.str)) {
      scl_log_fprintf(out, ERROR, "[%s] Failure in expected line %d: '",
                      t->name, line);
      str_print(out, expected.str);
      scl_log_fprintf(out, ERROR, "' != ' ");
      str_print(out, input.str);
      scl_log_fprintf(out, ERROR, "'\n");
      state.err = ERR_TRIAL_FAILURE;
      break;
    }

    line++;
  }

  // lastly, the input data should now be EOF! if not we have an error
  if (trial_read_line_from(eio, out, &expected, FALSE) != EOF) {
    state.err = ERR_PROCESS_UNEXPECTED_DATE_END;
  }

  strbuf_free(&input);
  strbuf_free(&expected);

  state.exit = pclose(pio);
  fclose(eio);

  if (state.exit != 0) {
    scl_log_fprintf(out, INFO, "[%s] Exit code is %d\n", t->name, exit);
  }

  state.success = state.exit == 0 && state.err == OK;

  if (!state.success) {
    scl_log_fprintf(stderr, ERROR, "[%s] %s\n", t->name,
                    error_to_str(state.err));
  }

  scl_log_fprintf(out, OUTPUT, "[%s] %s\n", t->name,
                  state.success ? "PASSED" : "FAILED");

  return state;
}

void trial_print(Trial *t, FILE *f) {
  scl_log_fprintf(f, DEBUG, "[NAME]: %s\n", t->name);
  scl_log_fprintf(f, DEBUG, "[COMMAND]: %s\n", t->command);
  scl_log_fprintf(f, DEBUG, "[BEGIN]: %s\n", t->begin);
  scl_log_fprintf(f, DEBUG, "[END]: %s\n", t->end);
  scl_log_fprintf(f, DEBUG, "[ECHO]: %d\n", t->echo);
  scl_log_fprintf(f, DEBUG, "[DATA PATH]: %s\n", t->data_path);
  scl_log_fprintf(f, DEBUG, "[LINE PREFIX]: %s\n", t->test_line_prefix);
}

void trial_free(Trial *trial) {
  if (trial->err) {
    return;
  }

  free(trial->begin);
  free(trial->end);
  free(trial->test_line_prefix);
  free(trial->data_path);
  free(trial->command);
  free(trial->name);
  free(trial->expected_path);
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
