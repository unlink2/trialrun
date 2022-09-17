#ifndef CONFIG_H_
#define CONFIG_H_

#include "types.h"

typedef struct Config {
  u8 log_level;
} Config;

extern Config global_cfg;

void cfg_init(Config *cfg);

// this macro obtains the config object and
// creates it in the local scope
#define cfg() Config *cfg = &global_cfg;

#endif
