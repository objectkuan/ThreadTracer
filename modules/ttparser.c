#include "ttparser.h"

// According to experiment, the traced funciton name 
// starts from the 49th character. This funciton tells
// whether this line contains a traced function name.
int compare_traced_function(const char* buf, const char* name) {
	int len = strlen(name);
	int i, j;
	int traced_function_offset = get_traced_function_offset(buf);
	if (traced_function_offset < -1) return 0;
	for (i = 0, j = traced_function_offset; i < len; ++i, ++j) {
		if (buf[j] != name[i]) 
			return 0;
	}
	return 1;
}

uint64_t get_timestamp(const char* buf) {
	uint64_t result = 0;
	const char* c;
	for (c = buf + TIMESTAMP_OFFSET; *c != ':'; ++c) {
		if (*c == '.') continue;
		result = result * 10 + (*c - '0');
	}
	return result;
}

uint64_t get_subject_thread_id(const char* buf) {
	uint64_t result = 0;
	const char* c;
	for (c = buf + 17; *c >= '0' && *c <= '9'; ++c) {
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
	int traced_function_offset = get_traced_function_offset(buf);
	// buf + traced_function_offset + strlen("sched_wakeup_new")
	// buf + traced_function_offset + 16
	return get_object_thread_id(buf + traced_function_offset + 16);
}
uint64_t get_thread_wakeup_wakenup_thread_id(const char* buf) {
	int traced_function_offset = get_traced_function_offset(buf);
	// buf + traced_function_offset + strlen("thread_wakeup")
	// buf + traced_function_offset + 13
	return get_object_thread_id(buf + traced_function_offset + 13);
}

// sys_futex
void get_futex_args_sys_futex(const char* buf, uint64_t* resource, int* opcode) {
	const char* c;
	int traced_function_offset = get_traced_function_offset(buf);
	*resource = 0;
	// buf + traced_function_offset + strlen("sys_futex(uaddr: ")
	// buf + traced_function_offset + 17
	for (c = buf + traced_function_offset + 17; in_hex_range(*c); ++c) {
		if (*c >= '0' && *c <= '9') 
			*resource = (*resource) * 16 + *c - '0';
		else
			*resource = (*resource) * 16 + *c - 'a' + 10;
	}

	*opcode = 0;
	// move offset ", op: "
	for (c += 6; *c != ','; ++c)
		if (*c >= '0' && *c <= '9') 
			*opcode = (*opcode) * 16 + *c - '0';
		else
			*opcode = (*opcode) * 16 + *c - 'a' + 10;
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
	const char* sub = strstr(buf, "next_pid=");
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
	int traced_function_offset = get_traced_function_offset(buf);
	// buf + traced_function_offset + strlen("sys_futex -> 0x")
	// buf + traced_function_offset + 15
	for (c = buf + traced_function_offset + 15; in_hex_range(*c); ++c) {
		if (*c >= '0' && *c <= '9') 
			result = result * 16 + *c - '0';
		else
			result = result * 16 + *c - 'a' + 10;
	}
	return result;
}




void parse_event(const char* buf) {
	if (!event_linked_list) event_linked_list = init_event_linked_list();
	// First to detect the traced function
	if (compare_traced_function(buf, "sched_wakeup_new")) { 
		uint64_t parent_thread_id = get_subject_thread_id(buf);
		uint64_t child_thread_id = get_sched_wakeup_new_child_thread_id(buf);
		uint64_t timestamp = get_timestamp(buf);
		
		// Event THREAD_CREATE
		event_node_t* node = create_thread_event(event_linked_list);
		thread_event* event = node->event;
		event->type = THREAD_CREATE;
		event->event.thread_create.parent_thread_id = parent_thread_id;
		event->event.thread_create.child_thread_id = child_thread_id;
		event->event.thread_create.timestamp = timestamp;
		insert_thread_event(node);

		start_record_thread(child_thread_id);
		wakeup_thread(child_thread_id);
		print_event("%" PRId64 " CREATE_WAKEUP %" PRId64 " %" PRId64 "\n", timestamp, parent_thread_id, child_thread_id);
	} else if (compare_traced_function(buf, "sched_wakeup")) {
		// wake up thread
		uint64_t from_thread_id = get_subject_thread_id(buf);
		uint64_t to_thread_id = get_thread_wakeup_wakenup_thread_id(buf);
		uint64_t timestamp = get_timestamp(buf);

		// Event THREAD_WAKEUP
		event_node_t* node = create_thread_event(event_linked_list);
		thread_event* event = node->event;
		event->type = THREAD_WAKEUP;
		event->event.thread_wakeup.from_thread_id = from_thread_id;
		event->event.thread_wakeup.to_thread_id = to_thread_id;
		event->event.thread_wakeup.timestamp = timestamp;
		insert_thread_event(node);

		wakeup_thread(to_thread_id);

		print_event("%" PRId64 " WAKEUP %" PRId64 " %" PRId64 "\n", timestamp, from_thread_id, to_thread_id);
	} else if (compare_traced_function(buf, "sys_nanosleep")) {
		int traced_function_offset = get_traced_function_offset(buf);
		int tail_offset = traced_function_offset + 13; // traced_function_offset + strlen("sys_nanosleep")
		if (buf[tail_offset] == '(') {
			// Call sys_nanosleep
			uint64_t thread_id = get_subject_thread_id(buf);
			uint64_t timestamp = get_timestamp(buf);

			// Event THREAD_SLEEP
			event_node_t* node = create_thread_event(event_linked_list);
			thread_event* event = node->event;
			event->type = THREAD_SLEEP;
			event->event.thread_sleep.thread_id = thread_id;
			event->event.thread_sleep.timestamp = timestamp;
			insert_thread_event(node);

			sleep_thread(thread_id);
			print_event("%" PRId64 " SLEEP %" PRId64 "\n", timestamp, thread_id);
		} else {
			// Return from sys_nanosleep
			// We don't care this sometimes 
			goto not_match;
		}
	} else if (compare_traced_function(buf, "sys_futex")) {
		int traced_function_offset = get_traced_function_offset(buf);
		int tail_offset = traced_function_offset + 9; // traced_function_offset + strlen("sys_futex")
		uint64_t timestamp = get_timestamp(buf);
		if (buf[tail_offset] == '(') {
			// Call sys_futex
			uint64_t thread_id = get_subject_thread_id(buf);
			uint64_t resource = 0;
			int opcode = 0;
			get_futex_args_sys_futex(buf, &resource, &opcode);
			assert(resource != 0);
			switch (opcode & 0x7f) {
				case 0: // wait for mutex
				{
					// Event THREAD_WAIT_FUTEX
					event_node_t* node = create_thread_event(event_linked_list);
					thread_event* event = node->event;
					event->type = THREAD_WAIT_FUTEX;
					event->event.thread_wait_futex.thread_id = thread_id;
					event->event.thread_wait_futex.resource_id = resource;
					event->event.thread_wait_futex.timestamp = timestamp;
					insert_thread_event(node);
					insert_futex_event(node);

					wait_resource_thread(thread_id, resource, timestamp);
					print_event("%" PRId64 " WAIT_FUTEX %" PRId64 " %" PRId64 "\n", timestamp, thread_id, resource);
					break;
				}
				case 1: // release mutex
				{
					// Event THREAD_RELEASE_FUTEX
					event_node_t* node = create_thread_event(event_linked_list);
					thread_event* event = node->event;
					event->type = THREAD_RELEASE_FUTEX;
					event->event.thread_release_futex.thread_id = thread_id;
					event->event.thread_release_futex.resource_id = resource;
					event->event.thread_release_futex.timestamp = timestamp;
					insert_thread_event(node);
					insert_futex_event(node);

					start_releasing_resource_thread(thread_id, resource);

					print_event("%" PRId64 " RELEASE_FUTEX %" PRId64 " %" PRId64 "\n", timestamp, thread_id, resource);
					break;
				}
				default:
					assert(0);
					break;
			}
		} else {
			// Return from sys_futex
			uint64_t thread_id = get_subject_thread_id(buf);
			int64_t retval = get_futex_retval_sys_futex(buf);
			thread_state* s = find_thread(thread_id);
			assert(s);
			assert(!(s->waiting_futex && s->releasing_futex));
			if (s->waiting_futex) {
				// get futex
				assert(retval == 0 || retval == -11);
				if (retval == 0 || retval == -11) {
					uint64_t resource = get_resource_thread(thread_id);

					// Event THREAD_GET_FUTEX
					event_node_t* node = create_thread_event(event_linked_list);
					thread_event* event = node->event;
					event->type = THREAD_GET_FUTEX;
					event->event.thread_get_futex.thread_id = thread_id;
					event->event.thread_get_futex.resource_id = resource;
					event->event.thread_get_futex.timestamp = timestamp;
					insert_thread_event(node);
					insert_futex_event(node);

					print_event("%" PRId64 " GET_FUTEX %" PRId64 "\n", timestamp, thread_id);
				}
			} else if (s->releasing_futex) {
				// end releasing futex
				end_releasing_resource_thread(thread_id);
				print_event("%" PRId64 " - WAKE_UP %" PRId64 " %d others\n", timestamp, thread_id, retval);
			}
		}		
	} else if (compare_traced_function(buf, "sched_process_exit")) {
		uint64_t timestamp = get_timestamp(buf);
		uint64_t thread_id = get_subject_thread_id(buf);

		// Event THREAD_EXIT
		event_node_t* node = create_thread_event(event_linked_list);
		thread_event* event = node->event;
		event->type = THREAD_EXIT;
		event->event.thread_exit.thread_id = thread_id;
		event->event.thread_exit.timestamp = timestamp;
		insert_thread_event(node);

		exit_thread(thread_id);
		print_event("%" PRId64 " EXIT %" PRId64 "\n", timestamp, thread_id);
	} else if (compare_traced_function(buf, "sched_switch")) {
		goto not_match;
		// TODO: should I care about switching
		uint64_t timestamp = get_timestamp(buf);
		uint64_t prev_thread_id = get_prev_thread_id_sched_switch(buf);	
		uint64_t next_thread_id = get_next_thread_id_sched_switch(buf);	
		switch_thread(prev_thread_id, next_thread_id);		
		print_event("%" PRId64 " SWITCH %" PRId64 " %" PRId64 "\n", timestamp, prev_thread_id, next_thread_id);
	} else {
		goto not_match;
	}
	print_thread_states();
	dump_all_event_lists();

not_match:
	fflush(stdout);
}




