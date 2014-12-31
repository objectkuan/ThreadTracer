#include "ttevent.h"

// According to experiment, the traced funciton name 
// starts from the 49th character. This funciton tells
// whether this line contains a traced function name.
int compare_traced_function(const char* buf, const char* name) {
	int len = strlen(name);
	int i, j;
	for (i = 0, j = 49; i < len; ++i, ++j) {
		if (buf[j] != name[i]) 
			return 0;
	}
	return 1;
}

uint64_t get_timestamp(const char* buf) {
	uint64_t result = 0;
	const char* c;
	for (c = buf + 35; *c != ':'; ++c) {
		if (*c == '.') continue;
		result = result * 10 + (*c - '0');
	}
	return result;
}

uint64_t get_subject_thread_id(const char* buf) {
	uint64_t result = 0;
	const char* c;
	for (c = buf + 18; *c >= '0' && *c <= '9'; ++c) {
		result = result * 10 + *c - '0';
	}
	return result;
}

uint64_t get_object_thread_id(const char* buf_with_offset) {
	const char* sub = strstr(buf_with_offset, "pid=");
	const char* c;
	assert(sub > 0);

	uint64_t result = 0;
	for (c = sub + 4; *c >= '0' && *c <= '9'; ++c) {
		result = result * 10 + *c - '0';
	}
	return result;
}

// sched_wakeup_new
uint64_t get_sched_wakeup_new_child_thread_id(const char* buf) {
	// buf + 49 + strlen("sched_wakeup_new")
	// buf + 49 + 16
	// buf + 65
	return get_object_thread_id(buf + 65);
}
uint64_t get_thread_wakeup_wakenup_thread_id(const char* buf) {
	// buf + 49 + strlen("thread_wakeup")
	// buf + 49 + 13
	// buf + 62
	return get_object_thread_id(buf + 62);
}

// sys_futex
void get_futex_args_sys_futex(const char* buf, uint64_t* resource, int* opcode) {
	const char* c;
	
	*resource = 0;
	// buf + 49 + strlen("sys_futex(uaddr: ")
	for (c = buf + 66; in_hex_range(*c); ++c) {
		if (*c >= '0' && *c <= '9') 
			*resource = (*resource) * 16 + *c - '0';
		else
			*resource = (*resource) * 16 + *c - 'a' + 10;
	}

	*opcode = 0;
	// move offset ", op: "
	for (c += 6; *c != ','; ++c)
		*opcode = (*opcode) * 10 + *c - '0';
}

// sys_sched_switch
uint64_t get_prev_thread_id_sched_switch(const char* buf) {
	const char* sub = strstr(buf, "prev_pid=");
	const char* c;
	assert(sub > 0);

	uint64_t result = 0;
	for (c = sub + 9; *c >= '0' && *c <= '9'; ++c) {
		result = result * 10 + *c - '0';
	}
	return result;
}
uint64_t get_next_thread_id_sched_switch(const char* buf) {
	const char* sub = strstr(buf, "new_pid=");
	const char* c;
	assert(sub > 0);

	uint64_t result = 0;
	for (c = sub + 9; *c >= '0' && *c <= '9'; ++c) {
		result = result * 10 + *c - '0';
	}
	return result;
}

uint64_t get_futex_retval_sys_futex(const char* buf) {
	uint64_t result = 0;
	const char* c;
	// buf + 49 + strlen("sys_futex -> 0x")
	for (c = buf + 62; in_hex_range(*c); ++c) {
		if (*c >= '0' && *c <= '9') 
			result = result * 16 + *c - '0';
		else
			result = result * 16 + *c - 'a' + 10;
	}
	return result;
}

void parse_event(const char* buf, struct thread_event* event) {
	// First to detect the traced function
	if (compare_traced_function(buf, "sched_wakeup_new")) { 
		uint64_t parent_thread_id = get_subject_thread_id(buf);
		uint64_t child_thread_id = get_sched_wakeup_new_child_thread_id(buf);
		uint64_t timestamp = get_timestamp(buf);

		event->type = THREAD_CREATE;
		event->event.thread_create.parent_thread_id = parent_thread_id;
		event->event.thread_create.child_thread_id = child_thread_id;
		event->event.thread_create.timestamp = timestamp;

		start_record_thread(child_thread_id);
		wakeup_thread(child_thread_id);
		FPRINT("%" PRId64 " Creates and wakeups %" PRId64 "-> %" PRId64 "\n", timestamp, parent_thread_id, child_thread_id);
	} else if (compare_traced_function(buf, "sched_wakeup")) {
		uint64_t from_thread_id = get_subject_thread_id(buf);
		uint64_t to_thread_id = get_thread_wakeup_wakenup_thread_id(buf);
		uint64_t timestamp = get_timestamp(buf);

		wakeup_thread(to_thread_id);
		FPRINT("%" PRId64 " Wakeups %" PRId64 "-> %" PRId64 "\n", timestamp, from_thread_id, to_thread_id);
	} else if (compare_traced_function(buf, "sys_nanosleep")) {
		int tail_offset = 62; // 49 + strlen("sys_nanosleep")
		if (buf[tail_offset] == '(') {
			// Call sys_nanosleep
			uint64_t thread_id = get_subject_thread_id(buf);
			uint64_t timestamp = get_timestamp(buf);
			FPRINT("%" PRId64 " %" PRId64 " Sleep\n", timestamp, thread_id);
			sleep_thread(thread_id);
		} else {
			// Return from sys_nanosleep
			// We don't care this sometimes 
		}
	} else if (compare_traced_function(buf, "sys_futex")) {
		int tail_offset = 58; // 49 + strlen("sys_futex")
		uint64_t timestamp = get_timestamp(buf);
		if (buf[tail_offset] == '(') {
			// Call sys_futex
			uint64_t thread_id = get_subject_thread_id(buf);
			uint64_t resource = 0;
			int opcode = 0;
			get_futex_args_sys_futex(buf, &resource, &opcode);
			assert(resource != 0);
			switch (opcode & 0x1) {
				case 0: // wait for mutex
					wait_resource_thread(thread_id, resource);	
					FPRINT("%" PRId64 " %" PRId64 " Waits for %" PRId64 "\n", timestamp, thread_id, resource);
					break;
				case 1: // release mutex
					release_resource_thread(thread_id, resource);
					FPRINT("%" PRId64 " %" PRId64 " Releases %" PRId64 "\n", timestamp, thread_id, resource);
					break;
				default:
					assert(0);
					break;
			}
		} else {
			// Return from sys_futex
			uint64_t thread_id = get_subject_thread_id(buf);
			uint64_t retval = get_futex_retval_sys_futex(buf);
			struct thread_state* s = find_thread(thread_id);
			assert(s);
			if (s->state == WAITING_FUTEX) {
				if (retval == 0) {
					get_resource_thread(thread_id);
					FPRINT("%" PRId64 " %" PRId64 " Gets futex\n", timestamp, thread_id);
				}
			}
		}		
	} else if (compare_traced_function(buf, "sched_process_exit")) {
		uint64_t thread_id = get_subject_thread_id(buf);
		exit_thread(thread_id);	
	} else if (compare_traced_function(buf, "sched_switch")) {
		uint64_t prev_thread_id = get_prev_thread_id_sched_switch(buf);		
		uint64_t next_thread_id = get_next_thread_id_sched_switch(buf);		
	}
}
