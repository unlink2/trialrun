#include "trial.h"
#include "config.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <scl/macros.h>
#include "str.h"
#include <scl/scl.h>

#define BEGIN "===BEGIN"
#define END "===END"

typedef struct TrialParseIni {
  SclIni ini;
  Trial *t;
} TrialParseIni;

void trial_state_init(TrialState *s) { memset(s, 0, sizeof(TrialState)); }

bool trial_bool_val(Str value, Error *err) {
  *err = OK;
  if (str_eq_raw(value, "true")) {
    return TRUE;
  } else if (str_eq_raw(value, "false")) {
    return FALSE;
  } else {
    char *failed_value = str_to_str_alloc(value, alloc());
    scl_log_fprintf(stderr, ERROR, "Value Error: %s %d\n", failed_value,
                    value.len);
    alloc().free(failed_value);
    *err = ERR_TRIAL_PARSER_VALUE_ERROR;
    return FALSE;
  }
}

int trial_parse_on_section(SclIni *ini, Str key) { return OK; }

int trial_parse_on_value(SclIni *ini, Str key, Str value) {
  TrialParseIni *tini = (TrialParseIni *)ini;
  Trial *t = tini->t;
  Error err = OK;

  // dumb check for each possible key
  // TODO there has to be a better way!
  if (str_eq_raw(key, "echo")) {
    t->echo = trial_bool_val(value, &err);
  } else if (str_eq_raw(key, "name")) {
    alloc().free(t->name);
    t->name = str_to_str_alloc(value, alloc());
  } else if (str_eq_raw(key, "command")) {
    alloc().free(t->command);
    t->command = str_to_str_alloc(value, alloc());
  } else if (str_eq_raw(key, "data")) {
    alloc().free(t->data_path);
    t->data_path = str_to_str_alloc(value, alloc());
  } else if (str_eq_raw(key, "expected")) {
    alloc().free(t->expected_path);
    t->expected_path = str_to_str_alloc(value, alloc());
  } else if (str_eq_raw(key, "test-line-prefix")) {
    alloc().free(t->test_line_prefix);
    t->test_line_prefix = str_to_str_alloc(value, alloc());
  } else if (str_eq_raw(key, "begin")) {
    alloc().free(t->begin);
    t->begin = str_to_str_alloc(value, alloc());
  } else if (str_eq_raw(key, "end")) {
    alloc().free(t->end);
    t->end = str_to_str_alloc(value, alloc());
  } else {
    char *failed_key = str_to_str_alloc(key, alloc());
    scl_log_fprintf(stderr, ERROR, "Key error '%s'\n", failed_key);
    alloc().free(failed_key);
    return ERR_TRIAL_PARSER_KEY_ERROR;
  }

  return err;
}

void trial_init(Trial *t) {
  memset(t, 0, sizeof(Trial));

  // set up defaults
  t->begin = str_from_alloc(BEGIN, alloc());
  t->end = str_from_alloc(END, alloc());
  t->test_line_prefix = str_from_alloc("", alloc());

  t->data_path = str_from_alloc(DEFAULT_PATH_OUT, alloc());
  t->echo = FALSE;
  t->expected_path = str_from_alloc(DEFAULT_PATH_IN, alloc());
}

Trial trial_from(char *input) {
  Trial trial;
  trial_init(&trial);

  // parse until the end of input

  TrialParseIni ini;
  ini.ini = scl_ini_init(trial_parse_on_value, trial_parse_on_section);
  ini.t = &trial;

  Str s = str_init(input, strlen(input));
  SclIniRes ini_res = scl_ini_parse((SclIni *)&ini, s);
  if (ini_res.err) {
    trial.err = ERR_TRIAL_PARSE;
  }
  if (ini_res.usr_err) {
    trial.err = ini_res.usr_err;
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
  state.err = t->err;
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

  StrBuffer input = strbuf_init_alloc(32, alloc());
  StrBuffer expected = strbuf_init_alloc(32, alloc());

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
    state.err = ERR_TRIAL_FAILURE;
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

  alloc().free(trial->begin);
  alloc().free(trial->end);
  alloc().free(trial->test_line_prefix);
  alloc().free(trial->data_path);
  alloc().free(trial->command);
  alloc().free(trial->name);
  alloc().free(trial->expected_path);
}

#ifdef TEST

#include <scl/macros.h>

void test_trial_parse_next(void **state) {}

#endif
