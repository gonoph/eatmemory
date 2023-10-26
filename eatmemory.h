#ifndef EATMEMORY_H
#define EATMEMORY_H

extern const int CHUNK;
#define ENV_MEMORY "EATMEGS"

typedef struct eat_flags_s eat_flags_t;
struct eat_flags_s {
  unsigned int megs;
  int flag_pct;
  int flag_quiet;
  int flag_free;
  int cgroup;
  unsigned long memtotal;
  unsigned long allocated;
};

extern eat_flags_t eat_flags;

#endif
