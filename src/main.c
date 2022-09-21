
/**
 * When built without test
 */
#include "config.h"
#include "log.h"
#include <string.h>
#ifndef TEST

/// only use main if binary
#if TYPE == bin

#include <stdio.h>
#include <stdlib.h>
#include <argp.h>

const char *argp_program_version = "trialrun 0.1";
const char *argp_program_bug_address = "<lukas@krickl.dev>";
static char doc[] = "trialrun";
static char args_doc[] = "";

static struct argp_option options[] = {
    {"verbose", 'v', NULL, 0, "Enable verbose output"},
    {"input", 'i', "PATH", 0, "The input trialrun file or directory"},
    {"output", 'o', "PATH", 0, "The output file"},
    {0}};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  Config *cfg = state->input;
  switch (key) {
  case 'v':
    cfg->log_level = LOG_LEVEL_LEN;
    break;
  case 'i':
    cfg->in_path = arg;
    break;
  case 'o':
    cfg->out_path = arg;
    break;
  case ARGP_KEY_ARG:
    if (state->arg_num >= 0) {
      /* Too many arguments. */
      argp_usage(state); // NOLINT
    }

    // arguments->args[state->arg_num] = arg;

    break;
  case ARGP_KEY_END:
    if (state->arg_num < 0) {
      /* Not enough arguments. */
      argp_usage(state); // NOLINT
    }
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

int main(int argc, char **argv) {
  Config *cfg = cfg();
  cfg_init(cfg);
  argp_parse(&argp, argc, argv, 0, 0, cfg); // NOLINT

  return run_tests(cfg);
}

#endif
#endif

/**
 * When built with test
 */
#ifdef TEST

#include "macros.h"
#include "error.h"
#include "trial.h"
#include "buffer.h"

int main(int argc, char **argv) {
  const struct CMUnitTest tests[] = {cmocka_unit_test(test_trial_parse_next),
                                     cmocka_unit_test(test_buffer_resize)};
  return cmocka_run_group_tests(tests, NULL, NULL);
}

#endif
