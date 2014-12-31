#include "ttthread_state.h"

struct thread_state* start_record_thread(uint64_t thread_id) {
	assert(thread_amount < MAX_THREAD_NUM);
	struct thread_state* state = &(thread_states[thread_amount]);
	state->thread_id = thread_id;
	state->state = UNKNOWN;
	state->waiting_futex = NOFUTEX;
	++thread_amount;
	return state;
}

struct thread_state* start_record_unknown_thread(uint64_t thread_id) {
	return start_record_thread(thread_id);
}

struct thread_state* find_thread(uint64_t thread_id) {
	int i;
	for(i = 0; i < thread_amount; i++) {
		if (thread_states[i].thread_id == thread_id)
			return &(thread_states[i]);
	}
	return NULL;
}

struct thread_state* change_thread_state(uint64_t thread_id, thread_running_state state) {
	struct thread_state* s = find_thread(thread_id);
	if (s == NULL) 
		s = start_record_unknown_thread(thread_id);
	s->state = state;
	return s;
}

void sleep_thread(uint64_t thread_id) {
	change_thread_state(thread_id, SLEEPING);
}

void wakeup_thread(uint64_t thread_id) {
	change_thread_state(thread_id, RUNNING);
}

void wait_resource_thread(uint64_t thread_id, uint64_t resource_id) {
	struct thread_state* s = change_thread_state(thread_id, WAITING_FUTEX);
	s->waiting_futex = resource_id;
}

void release_resource_thread(uint64_t thread_id, uint64_t resource_id) {
	struct thread_state* s = change_thread_state(thread_id, RUNNING);
	s->waiting_futex = NOFUTEX;
}

void get_resource_thread(uint64_t thread_id) {
	struct thread_state* s = change_thread_state(thread_id, RUNNING);
}

void exit_thread(uint64_t thread_id) {
	change_thread_state(thread_id, EXIT);
}
