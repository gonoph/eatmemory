/* eatmemory.c created sometime early 2003 by billy@gonoph.net */
/* Copyright (2010) Added ability to read command line */
/* Copyright (2023) brought it up to current C standards, added container file, signal catching, argument parsing */
/* released under The Unlicense for anyone silly enough to run it */
/* use at your own risk! */
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <string.h>
#include <getopt.h>
#include <unistd.h>

#include "malloc.h"
#include "args.h"
#include "cleanup.h"
#include "error.h"
#include "sys_info.h"

int main(int argc,char **argv)
{
    unsigned long looper1;
    fd_set empty_stdin, empty_stdout, empty_stderr;
    struct timeval waittv;

    cleanup_init();

    argparse(argc, argv);

    info("Consuming %u megabytes\n", eat_flags.megs);

    if (eat_flags.flag_random)
        info("+ enabled randomizing memory\n");

    if (sys_info_cgroup)
        info("+ detected running in a cgroup with memory limits.\n");

    bigarray=CreateLargeArray(eat_flags.megs);  /* create my array of chunks */

    CreateLargeChunks(eat_flags.megs, bigarray); /* fill in the array with actual chunks */

    info("+ Created %u megabytes\n", eat_flags.megs);
    info("+ Allocated %lu bytes\n", eat_flags.allocated);

    /* loop the memory to keep it out of swap
     * wait 100ms per chunk to keep machine from
     * freaking out with max processor usage -
     * especially if it starts to swap */

    unsigned int counter = 0;
    unsigned long refresh = 0;
    for (;;)
    {
        if (eat_flags.flag_random) {
            refresh = mksalt();
        };
        for (looper1=0;looper1<eat_flags.megs;looper1++)
        {
            if (eat_flags.flag_random) {
                unsigned long * lbuff = (unsigned long*) bigarray[looper1];
                int max = CHUNK / sizeof(unsigned long);
                for (int i = 0; i < max; i++) {
                    *lbuff = *lbuff ^ refresh;
                    lbuff++;
                }
            } else {
                memset(bigarray[looper1],'A' + (looper1 % 26) ,CHUNK); 
            }
            FD_ZERO(&empty_stdin);
            FD_ZERO(&empty_stdout);
            FD_ZERO(&empty_stderr);
            waittv.tv_sec = 0;
            waittv.tv_usec = 1000000 / 100; /* microseconds to 1/100th of a second */
            select(0,&empty_stdin,&empty_stdout,&empty_stderr,&waittv);
        }
        info("++ Looped: %u\n", ++counter);
    }

    return(0);
}

// vim: sw=4 cindent expandtab tabstop=4 :
