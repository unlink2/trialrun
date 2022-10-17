#include "config.h"
#include "trial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <scl/scl.h>

Config global_cfg;

void cfg_init(Config *cfg) {
  cfg->log_level = ERROR;
  cfg->in_path = DEFAULT_PATH_IN;
  cfg->out_path = DEFAULT_PATH_OUT;
  scl_log_set_level(cfg->log_level);
  cfg->alloc = scl_default_alloc();
}

FILE *open_output(Config *cfg) {
  if (scl_strncmp(cfg->out_path, DEFAULT_PATH_OUT,
                  scl_strlen(DEFAULT_PATH_OUT)) == 0) {
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

Error tr_run_tests(Config *cfg) {
  FILE *trial_file = fopen(cfg->in_path, "re");
  FILE *out = open_output(cfg);

  if (trial_file == NULL) {
    scl_log_fprintf(stderr, ERROR, "File IO Error: %s\n", cfg->in_path);
    return ERR_FILE_OPEN;
  }

  if (out == NULL) {
    scl_log_fprintf(stderr, ERROR, "File IO Error: %s\n", cfg->out_path);
    return ERR_FILE_OPEN;
  }
  Error e = tr_run_test(cfg, trial_file, out);

  close_output(out);
  fclose(trial_file);
  return e;
}

char *file_read_all(FILE *f) {
  fseek(f, 0, SEEK_END);
  usize len = ftell(f);
  rewind(f);

  char *buffer = alloc().malloc(len + 1);
  scl_memset(buffer, 0, len + 1);

  fread(buffer, len, 1, f);
  return buffer;
}

Error tr_run_test(Config *cfg, FILE *f, FILE *out) {
  char *buffer = file_read_all(f);
  // the entire file is in buffer now
  // parse it!

  Trial t = trial_from(buffer);
  TrialState s = trial_run(&t, out);

  trial_free(&t);
  alloc().free(buffer);
  return s.err;
}
