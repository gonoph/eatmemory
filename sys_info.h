#ifndef SYS_INFO_H
#define SYS_INFO_H

#define MEMPROC_TOTAL "MemTotal:"
#define MEMPROC_TOTAL_LEN sizeof(MEMPROC_TOTAL)-1
#define MEMPROC_AVAIL "MemAvailable:"
#define MEMPROC_AVAIL_LEN sizeof(MEMPROC_AVAIL)-1

extern int sys_info_cgroup;
unsigned long get_available_memory();
unsigned long get_visible_memory();

#endif
