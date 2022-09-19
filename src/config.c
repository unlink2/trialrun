#include "config.h"
#include "log.h"

Config global_cfg;

void cfg_init(Config *cfg) {
  cfg->log_level = OUTPUT;
  cfg->in_path = DEFAULT_PATH_IN;
  cfg->out_path = DEFAULT_PATH_OUT;
}
