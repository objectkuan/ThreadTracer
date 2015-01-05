#ifndef TT_THREAD_STATE_
#define TT_THREAD_STATE_

#include <stdio.h>
#include <inttypes.h>
#include <assert.h>

// Thread states:
typedef enum {
	UNKNOWN,
	RUNNABLE,
	SLEEPING,
	EXIT
} thread_running_state;

#define NOFUTEX 0
typedef struct thread_event thread_event;
typedef struct thread_state {
	uint64_t thread_id;
	thread_running_state state;

	uint64_t futex; // NOFUTEX if not holding or waiting futex
	uint64_t waiting_from_time;
	int waiting_futex;
	int releasing_futex;

} thread_state;

static thread_state *current = NULL;
#define MAX_THREAD_NUM 1024
thread_state thread_states[MAX_THREAD_NUM];
int thread_amount;

// Return a thread's state with id `thread_id`
thread_state* find_thread(uint64_t thread_id);

// Thread event interface functions.
thread_state* start_record_thread(uint64_t thread_id);
void sleep_thread(uint64_t thread_id);
void wakeup_thread(uint64_t thread_id);
void wait_resource_thread(uint64_t thread_id, uint64_t resource_id, uint64_t timestamp);
void start_releasing_resource_thread(uint64_t thread_id, uint64_t resource_id);
void end_releasing_resource_thread(uint64_t thread_id);
uint64_t get_resource_thread(uint64_t thread_id);
void switch_thread(uint64_t prev_thread_id, uint64_t next_thread_id);
void exit_thread(uint64_t thread_id);

// Print a thread states to stdout
void print_thread_states();

#endif
