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
// Print a frontend friendly traced event
#define print_frontend_event(msg, args...) printf(msg, ## args)

inline void print_all_subprocesses(FILE* fp);
inline void print_all_event_lists(FILE* fp);


#if 0 
#define log_event(msg, args...) \
	(run_mode & MODE_MASK_EVENT) && print_event(msg, ## args)

// Print all thread states to stdout
void print_thread_states();

inline void print_all_event_lists();
inline void print_subprocesses();

inline void print_line(const char* buf) {
	if (run_mode & MODE_MASK_RAW) {
		printf("%s", buf);
		fflush(stdout);
	}
}

void dump_all(int force);

// Print frontend-friendly evevnts
// #define PRINT_EVENT_PREFIX "[Filter] \t"
#define PRINT_EVENT_PREFIX ""

// Print a thread event to a file
void print_thread_event(thread_event_t* event, FILE* fp);
// Print a thread event to stdout
inline void print_thread_event_to_stdout(thread_event_t* event);

// Dump linked list for debugging
void dump_event_linked_list(event_linked_list_t* list);

// For debugging
void dump_all_event_lists();
#endif

#endif
