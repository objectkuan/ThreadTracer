#ifndef TT_DEPSOLVER_
#define TT_DEPSOLVER_

#include <stdio.h>
#include "ttevent.h"
#include "ttthread_state.h"

/**
 * We use two list to record the trace events. The first a list of events 
 * organized by events, and the second is organized by futex.
 * The thread_event_lists:
 *    Each thread_event_list records all the events of a certain thread marked
 *    by its thread_id.
 * The futex_event_lists:
 *    Each futex_event_list records all the events that happened on a futex, 
 *    including ENTER_FUTEX, EXIT_FUETX.
 */

// TODO: may overflow
#define MAX_THREAD_EVENTS 6650
typedef struct thread_event_list {
	uint64_t thread_id;
	int amount;
	event_linked_list_t* list;
} thread_event_list_t;

typedef struct futex_event_list {
	uint64_t futex;
	int amount;
	event_linked_list_t* list;
} futex_event_list_t;

typedef struct poll_event_list {
	uint64_t pollfd;
	int amount;
	event_linked_list_t* list;
} poll_event_list_t;


thread_event_list_t* find_event_list_by_thread(uint64_t thread_id);
void insert_thread_event(thread_event_t* event);

futex_event_list_t* find_event_list_by_futex(uint64_t futex);
void insert_futex_event(thread_event_t* event);

poll_event_list_t* find_event_list_by_pollfd(uint64_t pollfd);
void insert_poll_event(thread_event_t* event);

// Return a linked list of events within thread `thread_id`
// from timestamp `from` to `to`
event_linked_list_t* extrace_get_futex_events_in_range(uint64_t thread_id, 
		uint64_t from, uint64_t to);

#endif
