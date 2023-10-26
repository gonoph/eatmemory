#ifndef EAT_MALLOC_H
#define EAT_MALLOC_H

char **CreateLargeArray(unsigned long megs);
char **CreateLargeChunk(unsigned long chunks,char **largearray);

#endif
