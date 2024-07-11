#ifndef EAT_MALLOC_H
#define EAT_MALLOC_H

extern const int CHUNK;
extern char **bigarray;
char **CreateLargeArray(unsigned long megs);
char **CleanLargeArray(char **largearray);
char **CreateLargeChunks(unsigned long chunks,char **largearray);
char **CleanLargeChunks(unsigned long chunks, char **largearray);
unsigned long mksalt(void);

#endif
