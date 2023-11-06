#ifndef EAT_ERROR_H
#define EAT_ERROR_H

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#define err(...) {fprintf(stderr, __VA_ARGS__); if (errno > 0) perror(NULL); exit(-1);}
#define perr(msg) {perror(msg); exit(-1);}
#define info(...) {if (!eat_flags.flag_quiet) printf(__VA_ARGS__);fflush(stdout);}

#endif
