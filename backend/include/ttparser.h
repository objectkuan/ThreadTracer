#ifndef TT_PARSER_
#define TT_PARSER_

#include <inttypes.h>
#include <string.h>
#include "ttevent.h"
#include "ttthread_state.h"
#include "ttdepsolver.h"
#include "ttprocesses.h"
#include "ttlogger.h"

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

void init_parser(const uint64_t* pids, int n, int run_mode, const char* outdir, uint64_t period);
void parse_event(const char* buf);
static inline int in_hex_range(char c) {
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

static event_linked_list_t* event_linked_list = NULL;


/**
 * Logging
 */
#define MODE_MASK_RAW         1
#define MODE_MASK_EVENT       2
#define MODE_MASK_SUBPROCESS  4
#define MODE_MASK_EVENT_STAT  8
static int run_mode;

#define parser_print_raw(line_buf) \
	do { \
		if (run_mode & MODE_MASK_RAW) \
			print_raw_line(stdout, line_buf); \
	} while(0)

#define parser_print_frontend_event(timestamp, fe_event) \
	do { \
		if (run_mode & MODE_MASK_EVENT) \
			print_frontend_event(timestamp, fe_event); \
	} while(0)

#define parser_print_subprocesses() \
	do { \
		if (run_mode & MODE_MASK_SUBPROCESS) \
			print_all_subprocesses(stdout); \
	} while(0)

#define parser_print_event_list() \
	do { \
		if (run_mode & MODE_MASK_EVENT_STAT) \
			print_all_event_lists(stdout); \
	} while(0)


#endif
