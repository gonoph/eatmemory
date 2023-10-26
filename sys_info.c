#include "sys_info.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// tracking if we're in a cgroup
int sys_info_cgroup = 0;

unsigned long read_from_stream(FILE * __restrict stream, const char * __restrict pattern, size_t pattern_len) {
    char *BUFFER = NULL;
    size_t len = 0;
    ssize_t nread = 0;
    unsigned long number = 0;
    int matches = 0;

    while( (nread = getline(&BUFFER, &len, stream)) != -1) {
        if (!pattern) break;
        if (strncmp(pattern, BUFFER, pattern_len) == 0) break;
    }

    if (pattern) {
        matches = sscanf((BUFFER+pattern_len), "%lu", &number);
    } else {
        matches = sscanf(BUFFER, "%lu", &number);
    }

    if (matches == 0 || matches == EOF) return 0;

    return number;
}

unsigned long read_from_file(const char * __restrict src, const char * __restrict pattern, size_t pattern_len) {
    FILE * stream = fopen(src, "r");
    if (stream == NULL) {
        return 0;
    };
    unsigned long number = read_from_stream(stream, pattern, pattern_len);
    fclose(stream);
    return number;
}

unsigned long read_from_pipe(const char * __restrict src, const char * __restrict pattern, size_t pattern_len) {
    FILE * stream = popen(src, "r");
    if (stream == NULL) {
        return 0;
    };
    unsigned long number = read_from_stream(stream, pattern, pattern_len);
    pclose(stream);
    return number;
}

// returns the current visible available memory (cgroups may affect this)
unsigned long get_available_memory() {
#if defined(__linux__)
    // attempt to read from cgroup
    unsigned long current = read_from_file("/sys/fs/cgroup/memory.current", NULL, 0);
    if (current) {
        unsigned long max = get_visible_memory();
        return (max - (current >> 20) );
    }

    unsigned long avail = read_from_file("/proc/meminfo", MEMPROC_AVAIL, MEMPROC_AVAIL_LEN);
    if (avail) return avail >> 10;

    perr("[error] unable to parse contents of /proc/meminfo or cgroup /sys/fs/cgroup/memory.current");
#elif defined(__APPLE__)
    // read from sysctl
    unsigned long free = read_from_pipe("sysctl -n vm.page_free_count", NULL, 0);
    if (!free)
        perr("[error] unable to parse sysctl -n vm.page_free_count");

    unsigned long pagesize = sysconf(_SC_PAGESIZE);

    return (free * pagesize) >> 20;
#else
#error get_visible_memory() - Unsupported architecture
#endif
}

// returns the total memory visible (cgroups may make this lower than physical
// memory)
unsigned long get_visible_memory() {
#if defined(__linux__)
    // try cgroup 1st
    unsigned long bytes = read_from_file("/sys/fs/cgroup/memory.max", NULL, 0);
    if (bytes) {
        sys_info_cgroup = 1;
        return bytes >> 20;
    };

    // try meminfo
    unsigned long kilobytes = read_from_file("/proc/meminfo", MEMPROC_TOTAL, MEMPROC_TOTAL_LEN);
    if (kilobytes) {
        return kilobytes >> 10;
    };

    perr("[error] unable to parse MemTotal from /proc/meminfo nor from cgroups /sys/fs/cgroup/memory.max\n");
#elif defined(__APPLE__)
    // try reading sysctl
    unsigned long bytes = read_from_pipe("sysctl -n hw.memsize", NULL, 0);
    if (bytes) {
        return bytes >> 20;
    }

    perr("[error] unable to parse sysctl output as a number");
#else
#error get_visible_memory() - Unsupported architecture
#endif
}
// vim: sw=4 cindent expandtab tabstop=4 :
