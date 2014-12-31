#ifndef TT_THREAD_STATE_
#define TT_THREAD_STATE_

#include <stdio.h>
#include <stdint.h>
#include <assert.h>

// Thread states:
typedef enum {
	UNKNOWN,
	WAITING_FUTEX,
	SLEEPING,
	RUNNING,
	EXIT
} thread_running_state;
#define NOFUTEX 0
struct thread_state {
	uint64_t thread_id;
	thread_running_state state;
	uint64_t waiting_futex;

};
#define MAX_THREAD_NUM 1024
struct thread_state thread_states[MAX_THREAD_NUM];
int thread_amount;

struct thread_state* start_record_thread(uint64_t thread_id);
struct thread_state* find_thread(uint64_t thread_id);
struct thread_state* change_thread_state(uint64_t thread_id, thread_running_state state);


void sleep_thread(uint64_t thread_id);
void wakeup_thread(uint64_t thread_id);
void wait_resource_thread(uint64_t thread_id, uint64_t resource_id);
void release_resource_thread(uint64_t thread_id, uint64_t resource_id);
void get_resource_thread(uint64_t thread_id);
void exit_thread(uint64_t thread_id);

#endif
