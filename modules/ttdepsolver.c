#include "ttdepsolver.h"

thread_event_list_t* find_event_list_by_thread(uint64_t thread_id) {
	int i;
	for (i = 0; i < thread_event_list_len; i++) {
		if (thread_event_lists[i].thread_id == thread_id)
			return &(thread_event_lists[i]);
	}
	thread_event_list_t* list = &(thread_event_lists[thread_event_list_len++]);
	list->thread_id = thread_id;
	list->amount = 0;
	return list;
}
futex_event_list_t* find_event_list_by_futex(uint64_t futex) {
	int i;
	for (i = 0; i < futex_event_list_len; i++) {
		if (futex_event_lists[i].futex == futex)
			return &(futex_event_lists[i]);
	}
	futex_event_list_t* list = &(futex_event_lists[futex_event_list_len++]);
	list->futex = futex;
	list->amount = 0;
	return list;
}

void insert_thread_event(event_node_t* node) {
	uint64_t thread_id;
	thread_event_list_t* list;
	// find the thread_id
	switch(node->event->type) {
		case THREAD_CREATE:
			/*thread_id = node->event->event.thread_create.child_thread_id;*/
			return;
			break;
		case THREAD_WAIT_FUTEX:
			thread_id = node->event->event.thread_wait_futex.thread_id;
			break;
		case THREAD_GET_FUTEX:
			thread_id = node->event->event.thread_get_futex.thread_id;
			break;
		case THREAD_RELEASE_FUTEX:
			thread_id = node->event->event.thread_release_futex.thread_id;
			break;
		case THREAD_SLEEP:
			/*thread_id = node->event->event.thread_sleep.thread_id;*/
			return;
			break;
		case THREAD_WAKEUP:
			/*thread_id = node->event->event.thread_wakeup.to_thread_id;*/
			return;
			break;
		case THREAD_EXIT:
			thread_id = node->event->event.thread_exit.thread_id;
			break;
		default:
			assert(0);
	}
	list = find_event_list_by_thread(thread_id);
	list->thread_events[list->amount++] = node;
}

void insert_futex_event(event_node_t* node) {
	uint64_t futex;
	futex_event_list_t* list;
	// find the thread_id
	switch(node->event->type) {
		case THREAD_WAIT_FUTEX:
			futex = node->event->event.thread_wait_futex.resource_id;
			break;
		case THREAD_GET_FUTEX:
			futex = node->event->event.thread_get_futex.resource_id;
			break;
		case THREAD_RELEASE_FUTEX:
			futex = node->event->event.thread_release_futex.resource_id;
			break;
		case THREAD_CREATE:
		case THREAD_SLEEP:
		case THREAD_WAKEUP:
		case THREAD_EXIT:
		default:
			assert(0);
	}
	list = find_event_list_by_futex(futex);
	list->futex_events[list->amount++] = node;
}

event_linked_list_t* extract_get_futex_events_in_range(uint64_t thread_id, 
		uint64_t from, uint64_t to){
	thread_event_list_t* event_list = find_event_list_by_thread(thread_id);
	event_linked_list_t* rlist = init_event_linked_list();
	int i;
	for (i = 0; i < event_list->amount; i++) {
		thread_event* event = event_list->thread_events[i]->event;
		if (event->type == THREAD_GET_FUTEX) {
			insert_event_node_to_tail(rlist, event);
		}
	}
	return rlist;
}

thread_event* get_futex_release_event(thread_event* get_futex_event) {
	thread_event* result = NULL;
	uint64_t resource_id;
	int i;
	
	assert(get_futex_event->type == THREAD_GET_FUTEX);
	
	resource_id = get_futex_event->event.thread_get_futex.resource_id;
	futex_event_list_t* event_list = find_event_list_by_futex(resource_id);
	for (i = event_list->amount; i >= 0; i--) {
		thread_event* event = event_list->futex_events[i]->event;
		if (event->type == THREAD_RELEASE_FUTEX && event->event.thread_get_futex.timestamp <= get_futex_event->event.thread_get_futex.timestamp) {
			return event;	
		}
	}
	assert(0);
}

void dump_all_event_lists() {
	int i, j;
	printf("===============================\n");
	printf("Thread Events (%d):\n", thread_event_list_len);
	printf("-------------------------------\n");
	for (i = 0; i < thread_event_list_len; i++) {
		thread_event_list_t* list = &(thread_event_lists[i]);
		printf("\tThread %" PRId64 " (%d)\n", list->thread_id, list->amount);
		for (j = 0; j < list->amount; j++) {
			print_thread_event(list->thread_events[j]->event);
		}
	}
	printf("-------------------------------\n");
	printf("Futex Events (%d):\n", futex_event_list_len);
	printf("-------------------------------\n");
	for (i = 0; i < futex_event_list_len; i++) {
		futex_event_list_t* list = &(futex_event_lists[i]);
		printf("\tFutex %" PRId64 " (%d)\n", list->futex, list->amount);
		for (j = 0; j < list->amount; j++) {
			print_thread_event(list->futex_events[j]->event);
		}
	}
	printf("-------------------------------\n");
	printf("=============================\n");
	fflush(stdout);
}
