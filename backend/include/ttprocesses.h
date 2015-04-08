#ifndef TT_PROCESSES_
#define TT_PROCESSES_

#include <inttypes.h>

/**
 * Ftrace does not work well. Even though set_ftrace_pid is set, it will output
 * a lot of events that is not related to the process we care.
 */
#define MAX_SUBPROCESS 1024


int is_subprocess(uint64_t pid);
inline void push_subprocess(uint64_t pid);


#endif