#include "ttdepsolver.h"


int thread_event_list_len = 0;
int futex_event_list_len = 0;
int poll_event_list_len = 0;
thread_event_list_t thread_event_lists[MAX_THREAD_EVENTS];
futex_event_list_t futex_event_lists[MAX_THREAD_EVENTS];
poll_event_list_t poll_event_lists[MAX_THREAD_EVENTS];

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
	//return;
	uint64_t thread_id;
	thread_event_list_t* list;
	// find the thread_id
	switch(event->type) {
		case THREAD_CREATE:
			thread_id = event->event.thread_create.child_thread_id;
			break;
		case THREAD_ENTER_FUTEX:
			thread_id = event->event.thread_enter_futex.thread_id;
			break;
		case THREAD_EXIT_FUTEX:
			thread_id = event->event.thread_exit_futex.thread_id;
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

	// Skip the nonsense enter/exit pair
	if (!is_event_linkded_list_empty(list->list) &&
		(list->list->tail->event->type == THREAD_ENTER_POLL &&
			event->type == THREAD_EXIT_POLL ||
		list->list->tail->event->type == THREAD_ENTER_FUTEX &&
	    		event->type == THREAD_EXIT_FUTEX ||
		list->list->tail->event->type == THREAD_WAKEUP &&
	    		event->type == THREAD_WAKEUP
		)) {
		remove_event_node_from_tail(list->list);
		list->amount--;
		return;
	}
#if 0
	// Skip the timeout
	if (!is_event_linkded_list_empty(list->list)) {
		if (event->type == THREAD_EXIT_FUTEX &&
			event->event.thread_exit_futex.retval == -110) {
			assert(list->list->tail->event->type == THREAD_WAKEUP);
			remove_event_node_from_tail(list->list);
			list->amount--;
			assert(list->list->tail->event->type == THREAD_ENTER_FUTEX);
			remove_event_node_from_tail(list->list);
			list->amount--;
			return;
		}
	}
	// Skip short time
	if (!is_event_linkded_list_empty(list->list) &&
		event->type == THREAD_EXIT_POLL &&
		event->event.thread_exit_poll.sleep_time < 500000) {
		assert(list->list->tail->event->type == THREAD_WAKEUP);
		remove_event_node_from_tail(list->list);
		list->amount--;
		assert(list->list->tail->event->type == THREAD_ENTER_POLL);
		remove_event_node_from_tail(list->list);
		list->amount--;
		return;
	}
	if (!is_event_linkded_list_empty(list->list) &&
		event->type == THREAD_EXIT_FUTEX &&
		event->event.thread_exit_futex.sleep_time < 500000) {
		//printf("gigiwxx %" PRId64 "\n", event->event.thread_exit_futex.sleep_time);
		assert(list->list->tail->event->type == THREAD_WAKEUP);
		remove_event_node_from_tail(list->list);
		list->amount--;
		assert(list->list->tail->event->type == THREAD_ENTER_FUTEX);
		remove_event_node_from_tail(list->list);
		list->amount--;
		return;
	}
#endif

	//if (list->amount < MAX_THREAD_EVENTS) {
		insert_event_node_to_tail(list->list, event);
		list->amount++;
	//} else {
	//	printf("Thread events full\n");
	//	fflush(stdout);
	//}
}

void insert_futex_event(thread_event_t* event) {
	//return;
	uint64_t futex;
	futex_event_list_t* list;
	// find the thread_id
	switch(event->type) {
		case THREAD_ENTER_FUTEX:
			futex = event->event.thread_enter_futex.resource_id;
			break;
		case THREAD_EXIT_FUTEX:
			futex = event->event.thread_exit_futex.resource_id;
			break;
		default:
			assert(0);
	}
	list = find_event_list_by_futex(futex);
	//if (list->amount < MAX_THREAD_EVENTS) {
		insert_event_node_to_tail(list->list, event);
		list->amount++;
	//} else {
	//	printf("Futex events full\n");
	//	fflush(stdout);
	//}
}

void insert_poll_event (thread_event_t* event) {
	//return;
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
	//if (list->amount < MAX_THREAD_EVENTS) {
		insert_event_node_to_tail(list->list, event);
		list->amount++;
	//} else {
	//	printf("Poll events full\n");
	//	fflush(stdout);
	//}
}

event_linked_list_t* extract_get_futex_events_in_range(uint64_t thread_id, uint64_t from, uint64_t to){
	thread_event_list_t* event_list = find_event_list_by_thread(thread_id);
	event_linked_list_t* rlist = init_event_linked_list();
	event_node_t* node;
	for (node = event_list->list->head->next; node; node = node->next) {
		thread_event_t* event = node->event;
		if (event->type == THREAD_EXIT_FUTEX) {
			insert_event_node_to_tail(rlist, event);
		}
	}
	return rlist;
}
