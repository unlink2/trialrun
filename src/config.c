#include "config.h"
#include "log.h"
#include <stdio.h>
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
  return freopen(cfg->out_path, "we", stdout);
}

Errors run_tests(Config *cfg) { return OK; }
