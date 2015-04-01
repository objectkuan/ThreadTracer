#ifndef TT_PARSER_
#define TT_PARSER_

#include <inttypes.h>
#include <string.h>
#include "ttevent.h"
#include "ttthread_state.h"
#include "ttdepsolver.h"

/**
 * The timestamp does not have a fixed length, which will get longer as the
 * machine works. Here I assume that it always starts from the 35th character,
 * and the following function returns the offset of the traced function name.
 */
#define TIMESTAMP_OFFSET 34
static int traced_function_offset = TIMESTAMP_OFFSET;
inline static get_traced_function_offset(const char* buf) {
	int buflen = strlen(buf);
	for (; buf[traced_function_offset] != ':'
			&& traced_function_offset < buflen; )
		++traced_function_offset;
	if (traced_function_offset >= buflen) {
		traced_function_offset = TIMESTAMP_OFFSET;
		return -1;

	}
	return traced_function_offset + 2;

};

#define MODE_MASK_RAW		1
#define MODE_MASK_EVENT		2
#define MODE_MASK_SUBPROCESS	4
#define MODE_MASK_EVENT_STAT	8
static int run_mode;

void init_parser(uint64_t pid, int run_mode);
void parse_event(const char* buf);
static inline int in_hex_range(char c) {
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

static event_linked_list_t* event_linked_list = NULL;

/**
 * Ftrace does not work well. Even though set_ftrace_pid is set, it will output
 * a lot of events that is not related to the process we care.
 */
#define MAX_SUBPROCESS 1024
uint64_t root_pid; // the root process we care
uint64_t subprocess_amount;
uint64_t subprocess_ids[MAX_SUBPROCESS];

#define log_event(msg, args...) (run_mode & MODE_MASK_EVENT) && print_event(msg, ## args);

static int is_subprocess(uint64_t pid) {
	int i;
	for (i = 0; i < subprocess_amount; i++) 
		if (pid == subprocess_ids[i])
			return 1;
	return 0;
}
static inline void push_subprocess(uint64_t pid) {
	subprocess_ids[subprocess_amount++] = pid;
} 

static inline void print_subprocesses() {
	int i;
	if (run_mode & MODE_MASK_SUBPROCESS) {
		printf("========================\n");
		printf("Subprocesses\n");
		printf("------------------------\n");
		for (i = 0; i < subprocess_amount; i++) {
			printf("%" PRId64 "\t\t", subprocess_ids[i]);
			if (i % 5 == 4) printf("\n");
		}
		if (i % 5) printf("\n");
		printf("========================\n");
		fflush(stdout);
	}
}

static inline void print_line(const char* buf) {
	if (run_mode & MODE_MASK_RAW) {
		printf("%s", buf);
		fflush(stdout);

	}

}
static inline void print_all_event_lists() {
	if (run_mode & MODE_MASK_EVENT_STAT) {
		dump_all_event_lists();
	}

}

void dump_all(int force);


#endif
