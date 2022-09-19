#ifndef CONFIG_H_
#define CONFIG_H_

#include "error.h"
#include "types.h"
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
} Config;

extern Config global_cfg;

void cfg_init(Config *cfg);

// this macro obtains the config object and
// creates it in the local scope
#define cfg() Config *cfg = &global_cfg;

// Re-open stdout to output path if path is not -
FILE *open_output(Config *cfg);

// run all tests if path is a directory
// or a single test if path is a file
Errors run_tests(Config *cfg);

#endif
