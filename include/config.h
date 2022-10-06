#ifndef CONFIG_H_
#define CONFIG_H_

#include "error.h"
#include <scl/types.h>
#include <stdio.h>

#define DEFAULT_PATH_IN "-"
#define DEFAULT_PATH_OUT "-"

typedef struct Config {
  u32 log_level;
  // This can either be a directory of files
  // or a single file
  char *tr_path;

  char *in_path;
  char *out_path;
  Error overall;
} Config;

extern Config global_cfg;

void cfg_init(Config *cfg);

// this macro obtains the config object and
// creates it in the local scope
#define cfg() &global_cfg;

// Re-open stdout to output path if path is not -
FILE *open_output(Config *cfg);
void close_output(FILE *f);

char *file_read_all(FILE *f);

// run all tests if path is a directory
// or a single test if path is a file
Error tr_run_tests(Config *cfg);

// Run a single test
Error tr_run_test(Config *cfg, FILE *f, FILE *out);

#endif
