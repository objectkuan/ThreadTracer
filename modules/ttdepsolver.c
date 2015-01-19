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
	list->list = init_event_linked_list();
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
	list->list = init_event_linked_list();
	return list;
}
poll_event_list_t* find_event_list_by_pollfd(uint64_t pollfd) {
	int i;
	for (i = 0; i < poll_event_list_len; i++) {
		if (poll_event_lists[i].pollfd == pollfd)
			return &(poll_event_lists[i]);
	}
	poll_event_list_t* list = &(poll_event_lists[poll_event_list_len++]);
	list->pollfd = pollfd;
	list->amount = 0;
	list->list = init_event_linked_list();
	return list;
}


void insert_thread_event(thread_event_t* event) {
	uint64_t thread_id;
	thread_event_list_t* list;
	// find the thread_id
	switch(event->type) {
		case THREAD_CREATE:
			thread_id = event->event.thread_create.child_thread_id;
			break;
		case THREAD_WAIT_FUTEX:
			thread_id = event->event.thread_wait_futex.thread_id;
			break;
		case THREAD_GET_FUTEX:
			thread_id = event->event.thread_get_futex.thread_id;
			break;
		case THREAD_RELEASE_FUTEX:
			thread_id = event->event.thread_release_futex.thread_id;
			break;
		case THREAD_ENTER_POLL:
			thread_id = event->event.thread_enter_poll.thread_id;
			break;
		case THREAD_EXIT_POLL:
			thread_id = event->event.thread_exit_poll.thread_id;
			break;
		case THREAD_SLEEP:
			thread_id = event->event.thread_sleep.thread_id;
			break;
		case THREAD_WAKEUP:
			thread_id = event->event.thread_wakeup.to_thread_id;
			break;
		case THREAD_EXIT:
			thread_id = event->event.thread_exit.thread_id;
			break;
		default:
			assert(0);
	}
	list = find_event_list_by_thread(thread_id);
	if (list->amount < MAX_THREAD_EVENTS) {
		insert_event_node_to_tail(list->list, event);
		list->amount++;
	} else {
		printf("Thread events full\n");
		fflush(stdout);
	}
}

void insert_futex_event(thread_event_t* event) {
	uint64_t futex;
	futex_event_list_t* list;
	// find the thread_id
	switch(event->type) {
		case THREAD_WAIT_FUTEX:
			futex = event->event.thread_wait_futex.resource_id;
			break;
		case THREAD_GET_FUTEX:
			futex = event->event.thread_get_futex.resource_id;
			break;
		case THREAD_RELEASE_FUTEX:
			futex = event->event.thread_release_futex.resource_id;
			break;
		default:
			assert(0);
	}
	list = find_event_list_by_futex(futex);
	if (list->amount < MAX_THREAD_EVENTS) {
		insert_event_node_to_tail(list->list, event);
		list->amount++;
	} else {
		printf("Futex events full\n");
		fflush(stdout);
	}
}

void insert_poll_event (thread_event_t* event) {
	uint64_t pollfd;
	poll_event_list_t* list;
	// find the thread_id
	switch(event->type) {
		case THREAD_ENTER_POLL:
			pollfd = event->event.thread_enter_poll.resource_id;
			break;
		case THREAD_EXIT_POLL:
			pollfd = event->event.thread_exit_poll.resource_id;
			break;
		default:
			assert(0);
	}
	list = find_event_list_by_pollfd(pollfd);
	if (list->amount < MAX_THREAD_EVENTS) {
		insert_event_node_to_tail(list->list, event);
		list->amount++;
	} else {
		printf("Poll events full\n");
		fflush(stdout);
	}
}

event_linked_list_t* extract_get_futex_events_in_range(uint64_t thread_id, uint64_t from, uint64_t to){
	thread_event_list_t* event_list = find_event_list_by_thread(thread_id);
	event_linked_list_t* rlist = init_event_linked_list();
	event_node_t* node;
	for (node = event_list->list->head->next; node; node = node->next) {
		thread_event_t* event = node->event;
		if (event->type == THREAD_GET_FUTEX) {
			insert_event_node_to_tail(rlist, event);
		}
	}
	return rlist;
}

thread_event_t* get_futex_release_event(thread_event_t* get_futex_event) {
	thread_event_t* result = NULL;
	uint64_t resource_id;
	event_node_t* node;
	
	assert(get_futex_event->type == THREAD_GET_FUTEX);
	
	resource_id = get_futex_event->event.thread_get_futex.resource_id;
	futex_event_list_t* event_list = find_event_list_by_futex(resource_id);
	for (node = event_list->list->head->next; node; node = node->next) {
		thread_event_t* event = node->event;
		if (event->type == THREAD_RELEASE_FUTEX && event->event.thread_get_futex.timestamp <= get_futex_event->event.thread_get_futex.timestamp) {
			return event;	
		}
	}
	assert(0);
}

void dump_all_event_lists() {
	int i;
	event_node_t* node;
	printf("===============================\n");
	printf("Thread Events (%d):\n", thread_event_list_len);
	printf("-------------------------------\n");
	for (i = 0; i < thread_event_list_len; i++) {
		thread_event_list_t* list = &(thread_event_lists[i]);
		printf("\tThread %" PRId64 " (%d)\n", list->thread_id, list->amount);
		for (node = list->list->head->next; node; node = node->next) {
			print_thread_event(node->event);
		}
	}
	printf("-------------------------------\n");
	printf("Futex Events (%d):\n", futex_event_list_len);
	printf("-------------------------------\n");
	for (i = 0; i < futex_event_list_len; i++) {
		futex_event_list_t* list = &(futex_event_lists[i]);
		printf("\tFutex %" PRId64 " (%d)\n", list->futex, list->amount);
		for (node = list->list->head->next; node; node = node->next) {
			print_thread_event(node->event);
		}
	}
	printf("-------------------------------\n");
	printf("Poll Events (%d):\n", poll_event_list_len);
	printf("-------------------------------\n");
	for (i = 0; i < poll_event_list_len; i++) {
		poll_event_list_t* list = &(poll_event_lists[i]);
		printf("\tPoll fd %" PRId64 " (%d)\n", list->pollfd, list->amount);
		for (node = list->list->head->next; node; node = node->next) {
			print_thread_event(node->event);
		}
	}
	printf("=============================\n");
	fflush(stdout);
}
