#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "cleanup.h"
#include "malloc.h"
#include "args.h"

void sig_handler(int sig) {
    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGHUP, SIG_IGN);
    bigarray = CleanLargeChunks(eat_flags.megs, bigarray);
    bigarray = CleanLargeArray(bigarray);
    fprintf(stderr, "[exiting] Caught signal %d\n", sig);
    exit(0);
}

void cleanup_init(void) {
    struct sigaction new_action;
    new_action.sa_handler = sig_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;
    sigaction(SIGINT, &new_action, NULL);
    sigaction(SIGHUP, &new_action, NULL);
    sigaction(SIGTERM, &new_action, NULL);
}

// vim: sw=4 cindent expandtab tabstop=4 :
