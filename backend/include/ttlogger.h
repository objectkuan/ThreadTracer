#ifndef TT_LOGGER_
#define TT_LOGGER_

#include "ttevent.h"
#include "ttdepsolver.h"
#include "ttprocesses.h"
#include "ttthread_state.h"

// Simply print a raw traced event line
inline void print_raw_line(FILE* fp, const char* buf);
// Print a traced event
inline void print_event(FILE* fp, thread_event_t* event);

inline void print_all_subprocesses(FILE* fp);
inline void print_all_event_lists(FILE* fp);


/**
 * Print a frontend friendly traced event
 *
 * Once the timestamp reaches times of 5 seconds, 
 * contents will be dumped to a new file.
 */
static uint64_t last_times_5s = 0;
static unsigned int last_period = 0;
static FILE* curr_frontend_file = NULL;
void print_frontend_event(uint64_t timestamp, const char* fe_event);

#endif
