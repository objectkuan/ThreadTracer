#ifndef TT_EVENT_
#define TT_EVENT_

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>
#include <stdlib.h>



/**
 * Events:
 *
 * Each event type has a struct to store its details. And all event types are
 * collected into a struct called `thread_event`.
 *
 */
struct thread_create {
	uint64_t parent_thread_id;
	uint64_t child_thread_id;
	uint64_t timestamp;
}; // sched_wakeup_new

struct thread_enter_futex {
	uint64_t thread_id;
	uint64_t resource_id;
	uint64_t timestamp;
}; // sys_futex enter

struct thread_exit_futex {
	uint64_t thread_id;
	uint64_t resource_id;
	uint64_t retval;
	uint64_t sleep_time;
	uint64_t timestamp;
}; // sys_futex exit

struct thread_enter_poll {
	uint64_t thread_id;
	uint64_t resource_id;
	uint64_t timestamp;
}; // sys_poll enter

struct thread_exit_poll {
	uint64_t thread_id;
	uint64_t resource_id;
	uint64_t sleep_time;
	uint64_t timestamp;
}; // sys_poll exit

struct thread_sleep {
	uint64_t thread_id;
	uint64_t timestamp;
}; // sys_nanosleep

struct thread_wakeup {
	uint64_t from_thread_id;
	uint64_t to_thread_id;
	uint64_t timestamp;
}; // sched_wakeup

struct thread_exit {
	uint64_t thread_id;
	uint64_t timestamp;
}; // sched_process_exit

typedef enum {
	THREAD_CREATE,
	THREAD_ENTER_FUTEX,
	THREAD_EXIT_FUTEX,
	THREAD_ENTER_POLL,
	THREAD_EXIT_POLL,
	THREAD_SLEEP,
	THREAD_WAKEUP,
	THREAD_EXIT,
} event_type_t;
typedef struct thread_event {
	event_type_t type;
	union {
		struct thread_create thread_create;
		struct thread_enter_futex thread_enter_futex;
		struct thread_exit_futex thread_exit_futex;
		struct thread_enter_poll thread_enter_poll;
		struct thread_exit_poll thread_exit_poll;
		struct thread_sleep thread_sleep;
		struct thread_wakeup thread_wakeup;
		struct thread_exit thread_exit;
	} event;
} thread_event_t; // the collection of events

// Get a string of an event
int convert_thread_event_string(thread_event_t* event, char* result);


/* 
 * Event manager:
 * 
 * All events captured are stored in a linked list. They will be used later to
 * calculate the performance problem causes, or do something else.
 *
 */
typedef struct event_node_t {
	thread_event_t* event;
	struct event_node_t* next;
	struct event_node_t* prev;
} event_node_t; // Event linked list node type
typedef struct event_linked_list_t {
	event_node_t* head;
	event_node_t* tail;
} event_linked_list_t; // Event linked list type

// Create and initialize an event linked list
event_linked_list_t* init_event_linked_list();
// Insert a node to the tail of an event linked list
void insert_event_node_to_tail(event_linked_list_t* list, thread_event_t* event);
// Insert a node to the tail of an event linked list, and return
// the node to edit
event_node_t* create_thread_event(event_linked_list_t* list);
// Remove a node from tail
void remove_event_node_from_tail(event_linked_list_t* list);
// Check list empty
int is_event_linkded_list_empty(event_linked_list_t* list);


#endif
