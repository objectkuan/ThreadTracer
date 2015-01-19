#ifndef TT_DEPSOLVER_
#define TT_DEPSOLVER_

#include <stdio.h>
#include "ttevent.h"

/**
 * We use two list to record the trace events. The first a list of events 
 * organized by events, and the second is organized by futex.
 * The thread_event_lists:
 *    Each thread_event_list records all the events of a certain thread marked
 *    by its thread_id.
 * The futex_event_lists:
 *    Each futex_event_list records all the events that happened on a futex, 
 *    including GET_FUTEX, RELEASE_FUETX, WAIT_FUTEX.
 */

// TODO: may overflow
#define MAX_THREAD_EVENTS 6650
typedef struct thread_event_list {
	uint64_t thread_id;
	int amount;
	event_node_t* thread_events[MAX_THREAD_EVENTS];
} thread_event_list_t;

typedef struct futex_event_list {
	uint64_t futex;
	int amount;
	event_node_t* futex_events[MAX_THREAD_EVENTS];
} futex_event_list_t;

static int thread_event_list_len = 0;
static int futex_event_list_len = 0;
static thread_event_list_t thread_event_lists[MAX_THREAD_EVENTS];
static futex_event_list_t futex_event_lists[MAX_THREAD_EVENTS];

thread_event_list_t* find_event_list_by_thread(uint64_t thread_id);
void insert_thread_event(event_node_t* node);

futex_event_list_t* find_event_list_by_futex(uint64_t futex);
void insert_futex_event(event_node_t* node);

// Return a linked list of events within thread `thread_id`
// from timestamp `from` to `to`
event_linked_list_t* extrace_get_futex_events_in_range(uint64_t thread_id, 
		uint64_t from, uint64_t to);
thread_event_t* get_futex_release_event(thread_event_t* get_futex_event);

// For debugging
void dump_all_event_lists();

#endif
