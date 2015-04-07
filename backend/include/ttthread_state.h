#ifndef TT_THREAD_STATE_
#define TT_THREAD_STATE_

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

// Thread states:
typedef enum {
	UNKNOWN,
	RUNNABLE,
	SLEEPING,
	EXIT
} thread_running_state_t;

#define NOFUTEX 0
#define NOPOLL 0
typedef struct thread_event thread_event_t;
typedef struct thread_state {
	uint64_t thread_id;
	thread_running_state_t state;
	char thread_name[256];

	/* Resource waiting */
	uint64_t waiting_from_time;
	uint64_t sleep_time;
	//   futex:
	uint64_t futex; // NOFUTEX if not holding or waiting futex
	int waiting_futex;
	int releasing_futex;
	//   poll
	uint64_t pollfd; // NOPOLL if not waiting for a poll
	int waiting_pollfd;
	
} thread_state_t;



static thread_state_t *current = NULL;
#define MAX_THREAD_NUM 1024
thread_state_t thread_states[MAX_THREAD_NUM];
int thread_amount;




// Return a thread's state with id `thread_id`
thread_state_t* find_thread(uint64_t thread_id);
thread_state_t* name_thread(uint64_t thread_id, char* thread_name);
int name_changed(uint64_t thread_id, char* thread_name);

// Thread event interface functions.
thread_state_t* start_record_thread(uint64_t thread_id);
void sleep_thread(uint64_t thread_id, uint64_t timestamp);
void wakeup_thread(uint64_t thread_id, uint64_t timestamp);
void wait_futex_thread(uint64_t thread_id, uint64_t resource_id, uint64_t timestamp);
uint64_t get_futex_thread(uint64_t thread_id, uint64_t timestamp, uint64_t* sleep_time);
void wait_poll_thread(uint64_t thread_id, uint64_t pollfd, uint64_t timestamp);
uint64_t get_poll_thread(uint64_t thread_id, uint64_t timestamp, uint64_t* sleep_time);
void switch_thread(uint64_t prev_thread_id, uint64_t next_thread_id);
void exit_thread(uint64_t thread_id);

// Print all thread states to stdout
void print_thread_states();

#endif
