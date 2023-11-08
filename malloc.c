#include <string.h>

#include "error.h"
#include "args.h"

const int CHUNK = 1 << 20;
char **bigarray=NULL;

char **CreateLargeArray(unsigned long megs)
{
    char **largearray;

    largearray=calloc(megs, sizeof(char*));

    if (!largearray)
        err("[error] Unable to malloc() %lu megabytes.\n", megs);

    eat_flags.allocated += (megs * sizeof(char *));

    return(largearray);
}

char **CleanLargeArray(char **largearray)
{
    info("- Freeing chunk array: ");
    if (largearray) {
        free(largearray);
    }
    info("complete\n");
    return NULL;
}

// return a full unsigned long random number
unsigned long mksalt(void) {
    unsigned long salt;
    salt = (unsigned long) random() << 32;
    salt += (unsigned long) random();
    return salt;
}

char **CreateLargeChunks(unsigned long chunks,char **largearray)
{
    unsigned long looper1;

    /* Loop the largearray and create the CHUNKS
     * I did it this way as in theory, this app
     * should be able to consume >4GB of memory. */
    for(looper1=0;looper1<chunks;looper1++)
    {
        largearray[looper1]=malloc((CHUNK));
        if (!largearray[looper1]) {
            if (eat_flags.flag_pct && eat_flags.megs >= 100) {
                fprintf(stderr, "[warn] malloc unable to allocate chunk: ");
                perror(NULL);
                return(largearray);
            } else 
                err("[error] Unable to malloc() %lu chunks.\n", chunks);
    }

    if (eat_flags.flag_random) {
        // randomize the memory chunk
        unsigned long * lbuff = (unsigned long*) largearray[looper1];
        int max = CHUNK / sizeof(unsigned long);
        for (int i = 0; i < max; i++) {
            *lbuff = mksalt();
            lbuff++;
        }
    } else {
        /* set the memory to ascii(48) '0' */
        memset(largearray[looper1],'0',CHUNK);
    }

    eat_flags.allocated += CHUNK;
    }
    return(largearray);
}

char **CleanLargeChunks(unsigned long chunks, char **largearray)
{
    unsigned long looper1;
    info("-- Freeing %lu megs: ", chunks);
    for (looper1=0;looper1<chunks;looper1++)
    {
        free(largearray[looper1]);
        largearray[looper1]=NULL;
    }
    info("complete\n");
    return largearray;
}

// vim: sw=4 cindent expandtab tabstop=4 :
