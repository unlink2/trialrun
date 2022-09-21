#include "config.h"
#include "log.h"
#include "trial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Config global_cfg;

void cfg_init(Config *cfg) {
  cfg->log_level = ERROR;
  cfg->in_path = DEFAULT_PATH_IN;
  cfg->out_path = DEFAULT_PATH_OUT;
}

FILE *open_output(Config *cfg) {
  if (strncmp(cfg->out_path, DEFAULT_PATH_OUT, strlen(DEFAULT_PATH_OUT)) == 0) {
    return stdout;
  }
  return fopen(cfg->out_path, "we");
}

void close_output(FILE *f) {
  if (f == stdout) {
    return;
  }
  fclose(f);
}

Errors run_tests(Config *cfg) {
  FILE *trial_file = fopen(cfg->in_path, "re");
  FILE *out = open_output(cfg);

  if (trial_file == NULL) {
    tr_fprintf(stderr, ERROR, "File IO Error: %s\n", cfg->in_path);
    return ERR_FILE_OPEN;
  }

  if (out == NULL) {
    tr_fprintf(stderr, ERROR, "File IO Error: %s\n", cfg->out_path);
    return ERR_FILE_OPEN;
  }
  run_test(cfg, trial_file, out);

  close_output(out);
  fclose(trial_file);
  return OK;
}

char *file_read_all(FILE *f) {
  fseek(f, 0, SEEK_END);
  usize len = ftell(f);
  rewind(f);

  char *buffer = malloc(len + 1);
  memset(buffer, 0, len + 1);

  fread(buffer, len, 1, f);
  return buffer;
}

Errors run_test(Config *cfg, FILE *f, FILE *out) {
  char *buffer = file_read_all(f);
  // the entire file is in buffer now
  // parse it!

  Trial t = trial_from(buffer);
  trial_run(&t, out);

  trial_free(&t);
  free(buffer);
  return t.err;
}
