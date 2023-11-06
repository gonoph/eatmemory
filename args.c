#include "eatmemory.h"
#include "args.h"
#include "sys_info.h"
#include "error.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static const struct option longopts[] = {
  { "free",   no_argument,       NULL, 'f' },
  { "megs",   required_argument, NULL, 'm' },
  { "help",   no_argument,       NULL, 'h' },
  { "quiet",  no_argument,       NULL, 'q' },
  { "random", no_argument,       NULL, 'r' },
  { NULL,     0,                 NULL, 0 }
};

#define ARGS_DEFAULT "50%"

void help(void) {
  fprintf(stderr, "Usage: eatmemory [ -f | --free] [ -m metabytes | --megs megabytes ] [ -q | --quiet ] [ -h | --help ]\n\
    -f | --free  : attempt to only allocate free memory\n\
    -m | --megs  : the number of megabytes to consume (default %s)\n\
                   when %% is 100%% or more, all available RAM will be consumed.\n\
    -r | --random: randomize buffer in order to prevent memory compression\n\
    -q | --quiet : suppress output\n\
    -h | --help  : this help screen\n\
\n\
  ENVIRONMENT VARIABLES\n\
    %-13.13s: alternative to -m / --megs\n\
", ARGS_DEFAULT, ENV_MEMORY);
  exit(0);
}

void parse_megabytes(const char *source) {
    unsigned long megabytes = 0;
    char pct = 0;

    int matches = sscanf(source, "%lu%c", &megabytes, &pct);
    if (matches == 0 || matches == EOF)
        err("[error] unable to parse as a number: %s\n", source);

    // check if this is a percentage of memory
    if (pct == '%') {
        unsigned long total_memory = get_visible_memory();
        unsigned long avail_memory = get_available_memory();
        unsigned int pct = megabytes;
        if (eat_flags.flag_free) {
            if (pct >= 100) pct = 99;
            megabytes = (avail_memory) * pct / 100;
            info("Limiting to %d%% of availble free memory: free=%lu; total=%lu\n", pct, avail_memory, total_memory);
        } else {
            megabytes = total_memory * pct / 100;
        };
        info("using %u%% of %s memory %lu megabytes for a target allocation of %lu megabytes\n",
                pct, eat_flags.flag_free ? "available" : "total",
                eat_flags.flag_free ? avail_memory : total_memory, megabytes);
        if (megabytes < 1) megabytes = 1;
        eat_flags.flag_pct = 1;
    }

    eat_flags.megs = megabytes;
}

int argparse(int argc, char **argv) {
    int ch;
    char * saved_arg_megs = NULL;
    while ((ch = getopt_long(argc, argv, "fhqm:r", longopts, NULL)) != -1) {
        switch (ch) {
            case 'f':
                eat_flags.flag_free = 1;
                break;
            case 'q':
                eat_flags.flag_quiet = 1;
                break;
            case 'm':
                saved_arg_megs = (char*)strdup(optarg);
                break;
            case 'r':
                srandomdev();
                eat_flags.flag_random = 1;
                break;
            case 'h':
            default:
                help();
                break;
        }
    };

    if (saved_arg_megs) {
        parse_megabytes(saved_arg_megs);
        free(saved_arg_megs);
    };

    if (eat_flags.megs == 0) {
        // attempt getenv()
        char * env_eatmemory = getenv(ENV_MEMORY);
        if (env_eatmemory != NULL) {
            parse_megabytes(env_eatmemory);
        }
    };

    if (eat_flags.megs == 0) {
        // put in the default
        parse_megabytes(ARGS_DEFAULT);
    };

    if (eat_flags.megs == 0)
        err("[error] exhausted all parse options for megabytes\n");

    if (optind == argc-1) {
        help();
    };
    return optind;
}

// vim: sw=4 cindent expandtab tabstop=4 :
