#include "ttthread_state.h"

thread_state_t* start_record_thread(uint64_t thread_id) {
	assert(thread_amount < MAX_THREAD_NUM);
	thread_state_t* state = &(thread_states[thread_amount]);

	state->thread_id = thread_id;
	state->state = UNKNOWN;
	state->futex = NOFUTEX;
	state->waiting_futex = 0;
	state->releasing_futex = 0;
	state->pollfd = NOPOLL;
	state->waiting_pollfd = 0;
	state->waiting_from_time = 0;
	state->sleep_time = 0;
	++thread_amount;
	return state;
}

thread_state_t* start_record_unknown_thread(uint64_t thread_id) {
	return start_record_thread(thread_id);
}

thread_state_t* find_thread(uint64_t thread_id) {
	int i;
	for(i = 0; i < thread_amount; i++) {
		if (thread_states[i].thread_id == thread_id)
			return &(thread_states[i]);
	}
	return NULL;
}

thread_state_t* change_thread_state(uint64_t thread_id, thread_running_state_t state) {
	thread_state_t* s = find_thread(thread_id);
	if (s == NULL) 
		s = start_record_unknown_thread(thread_id);
	s->state = state;
	return s;
}

void sleep_thread(uint64_t thread_id, uint64_t timestamp) {
	thread_state_t* ts = change_thread_state(thread_id, SLEEPING);
	ts->sleep_time = timestamp;
}

void wakeup_thread(uint64_t thread_id, uint64_t timestamp) {
	thread_state_t* ts = change_thread_state(thread_id, RUNNABLE);
	if (ts->sleep_time > 0) {
		if (thread_id == 28343)
		printf("SLEEPINTERVAL %" PRId64 "\n", timestamp - ts->sleep_time);
		ts->sleep_time = 0;
	}
}

void wait_futex_thread(uint64_t thread_id, uint64_t resource_id, uint64_t timestamp) {
	thread_state_t* s = change_thread_state(thread_id, SLEEPING);
	s->futex = resource_id;
	s->waiting_from_time = timestamp;
	s->waiting_futex = 1;
	s->sleep_time = timestamp;
}

void start_releasing_futex_thread(uint64_t thread_id, uint64_t resource_id) {
	thread_state_t* s = find_thread(thread_id);
	if (s == NULL) {
		// A strange pid may be captured releasing a futex
		// since the ftrace pid filtering is not perfect
		s = start_record_unknown_thread(thread_id);
		s->state = RUNNABLE;
	}
	s->futex = NOFUTEX;
	s->waiting_from_time = 0;
	s->releasing_futex = 1;
}
void end_releasing_futex_thread(uint64_t thread_id) {
	thread_state_t* s = find_thread(thread_id);
	if (s)
		s->releasing_futex = 0;
}

uint64_t get_futex_thread(uint64_t thread_id) {
	thread_state_t* s = find_thread(thread_id);
	s->waiting_futex = 0;
	return s->futex;
}

void wait_poll_thread(uint64_t thread_id, uint64_t pollfd, uint64_t timestamp) {
	thread_state_t* s = change_thread_state(thread_id, SLEEPING);
	s->pollfd = pollfd;
	s->waiting_from_time = timestamp;
	s->waiting_pollfd = 1;
	s->sleep_time = timestamp;
}

uint64_t get_poll_thread(uint64_t thread_id) {
	uint64_t result;
	thread_state_t* s = find_thread(thread_id);
	result = s->pollfd;
	s->waiting_pollfd = 0;
	s->pollfd = NOPOLL;
	return result;
}

void switch_thread(uint64_t prev_thread_id, uint64_t next_thread_id) {
	thread_state_t* prev = find_thread(prev_thread_id);
	current = change_thread_state(next_thread_id, RUNNABLE);
}

void exit_thread(uint64_t thread_id) {
	change_thread_state(thread_id, EXIT);
}


inline const char* get_running_state(thread_running_state_t state) {
	switch(state) {
		case UNKNOWN:
			return "Unknown";
		case RUNNABLE:
			return "Runnable";
		case SLEEPING:
			return "Sleeping";
		case EXIT:
			return "Exit";
	}
}
void print_thread_states() {
	int i;
	printf("%d threads\n", thread_amount);
	printf("%" PRId64 " is current\n", current == NULL ? 0 : current->thread_id);
	for (i = 0; i < thread_amount; ++i) {
		thread_state_t* s = &(thread_states[i]);
		printf("%" PRId64 " %s ", s->thread_id, get_running_state(s->state));
		if (s->futex != NOFUTEX)
			printf("%s-%" PRId64, s->waiting_futex ? "waitingfor" : "holding", s->futex);
		printf("\n");
	}
}
