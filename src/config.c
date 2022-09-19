#include "config.h"
#include "log.h"
#include "trial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Config global_cfg;

void cfg_init(Config *cfg) {
  cfg->log_level = OUTPUT;
  cfg->in_path = DEFAULT_PATH_IN;
  cfg->out_path = DEFAULT_PATH_OUT;
}

FILE *open_output(Config *cfg) {
  if (strncmp(cfg->out_path, DEFAULT_PATH_OUT, strlen(DEFAULT_PATH_OUT)) == 0) {
    return stdout;
  }
  return fopen(cfg->out_path, "we");
}

void close_outpiut(FILE *f) {
  if (f == stdout) {
    return;
  }
  fclose(f);
}

Errors run_tests(Config *cfg) { return OK; }

Errors run_test(Config *cfg, FILE *f) {
  fseek(f, 0, SEEK_END);
  usize len = ftell(f);
  rewind(f);

  char *buffer = malloc(len + 1);
  memset(buffer, 0, len);

  fread(buffer, len, 1, f);
  // the entire file is in buffer now
  // parse it!

  FILE *out = open_output(cfg);
  Trial t = trial_from(buffer);
  trial_run(&t, out);
  close_outpiut(out);

  free(buffer);
  return t.err;
}
