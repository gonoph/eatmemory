#include <string.h>

#include "eatmemory.h"
#include "error.h"

char **CreateLargeArray(unsigned long megs)
{
    char **largearray;

    largearray=calloc(megs, sizeof(char*));

    if (!largearray)
        err("[error] Unable to malloc() %lu megabytes.\n", megs);

    eat_flags.allocated += (megs * sizeof(char *));

    return(largearray);
}

char **CreateLargeChunk(unsigned long chunks,char **largearray)
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

    /* set the memory to ascii(48) '0' */
    memset(largearray[looper1],'0',CHUNK);

    eat_flags.allocated += CHUNK;
    }
    return(largearray);
}

// vim: sw=4 cindent expandtab tabstop=4 :
