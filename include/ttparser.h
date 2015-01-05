#ifndef TT_PARSER_
#define TT_PARSER_

#include <inttypes.h>
#include <string.h>
#include "ttevent.h"
#include "ttthread_state.h"
#include "ttdepsolver.h"

/* The timestamp does not have a fixed length, which will get longer as the
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

void parse_event(const char* buf);
static inline int in_hex_range(char c) {
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

static event_linked_list_t* event_linked_list = NULL;

#endif
