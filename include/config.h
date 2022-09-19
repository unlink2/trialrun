#ifndef CONFIG_H_
#define CONFIG_H_

#include "types.h"

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

#endif
