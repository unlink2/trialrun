#include "config.h"
#include "log.h"

Config global_cfg;

void cfg_init(Config *cfg) { cfg->log_level = OUTPUT; }
