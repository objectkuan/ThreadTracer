#ifndef TT_EVENT_
#define TT_EVENT_

#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include "ttthread_state.h"

#define FPRINT(msg, args...) printf("[Filter] \t" msg, ## args);

// Events:
struct thread_create {
	uint64_t parent_thread_id;
	uint64_t child_thread_id;
	uint64_t timestamp;
}; // sched_wakeup_new

struct thread_wait_futex {
	uint64_t thread_id;
	uint64_t resource_id;
	uint64_t timestamp;
}; // sys_futex with op 80

struct thread_get_futex {
	uint64_t thread_id;
	uint64_t resource_id;
	uint64_t timestamp;
}; // sys_futex returning 0x0 and its last op is 80

struct thread_release_futex {
	uint64_t thread_id;
	uint64_t resource_id;
	uint64_t timestamp;
}; // sys_futex with op 81

struct thread_sleep {
	uint64_t thread_id;
	uint64_t timestamp;
}; // sys_nanosleep

struct thread_wakeup {
	uint64_t from_thread_id;
	uint64_t to_thread_id;
	uint64_t timestamp;
}; // sched_wakeup

typedef enum {
	THREAD_CREATE,
	THREAD_WAIT_FUTEX,
	THREAD_GET_FUTEX,
	THREAD_RELEASE_FUTEX,
	THREAD_SLEEP,
	THREAD_WAKEUP,
} event_type;
struct thread_event {
	event_type type;
	union {
		struct thread_create thread_create;
		struct thread_wait_futex thread_wait_futex;
		struct thread_get_futex thread_get_futex;
		struct thread_release_futex thread_release_futex;
		struct thread_sleep thread_sleep;
		struct thread_wakeup thread_wakeup;
	} event;
}; // the collection of events

int compare_traced_function(const char *buf, const char *name);
void parse_event(const char* buf, struct thread_event* event);

static inline int in_hex_range(char c) {
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

#endif
