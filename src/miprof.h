#pragma once

#include "sys/time.h"
#include "sys/resource.h"

// Profiling data structure
struct prof_data {
    char *command;
    struct timeval start_time;
    struct timeval end_time;
    struct rusage usage;
    long max_rss;
    int exit_status;
};

int miprof(int argc, char *argv[]);
