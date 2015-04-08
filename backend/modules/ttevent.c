#include "ttevent.h"

int convert_thread_event_string(thread_event_t* event, char* result) {
	int len = 0;
	switch(event->type) {
		case THREAD_CREATE:
		{
			len = sprintf(result, "%" PRId64 " CREATE_WAKEUP %" PRId64 " %" PRId64, 
				event->event.thread_create.timestamp, 
				event->event.thread_create.parent_thread_id, 
				event->event.thread_create.child_thread_id
			);
			break;
		}
		case THREAD_ENTER_FUTEX:
		{
			len = sprintf(result, "%" PRId64 " ENTER_FUTEX %" PRId64 " %" PRId64, 
				event->event.thread_enter_futex.timestamp, 
				event->event.thread_enter_futex.thread_id, 
				event->event.thread_enter_futex.resource_id
			);
			break;
		}
		case THREAD_EXIT_FUTEX:
		{
			len = sprintf(result, "%" PRId64 " EXIT_FUTEX %" PRId64 " %" PRId64 " SLT: %" PRId64, 
				event->event.thread_exit_futex.timestamp, 
				event->event.thread_exit_futex.thread_id,
				event->event.thread_exit_futex.retval,
				event->event.thread_exit_futex.sleep_time
			);
			break;
		}
		case THREAD_ENTER_POLL:
		{
			len = sprintf(result, "%" PRId64 " ENTER_POLL %" PRId64 " %" PRId64, 
				event->event.thread_enter_poll.timestamp, 
				event->event.thread_enter_poll.thread_id, 
				event->event.thread_enter_poll.resource_id
			);
			break;
		}
		case THREAD_EXIT_POLL:
		{
			len = sprintf(result, "%" PRId64 " EXIT_POLL %" PRId64 " SLT: %" PRId64, 
				event->event.thread_exit_poll.timestamp, 
				event->event.thread_exit_poll.thread_id,
				event->event.thread_exit_poll.sleep_time
			);
			break;
		}
		case THREAD_SLEEP:
		{
			len = sprintf(result, "%" PRId64 " SLEEP %" PRId64, 
				event->event.thread_sleep.timestamp, 
				event->event.thread_sleep.thread_id
			);
			break;
		}
		case THREAD_WAKEUP:
		{
			len = sprintf(result, "%" PRId64 " WAKEUP %" PRId64 " ==> %" PRId64, 
				event->event.thread_wakeup.timestamp, 
				event->event.thread_wakeup.from_thread_id, 
				event->event.thread_wakeup.to_thread_id
			);
			break;
		}
		case THREAD_EXIT:
		{
			len = sprintf(result, "%" PRId64 " EXIT %" PRId64, 
				event->event.thread_exit.timestamp, 
				event->event.thread_exit.thread_id);
			break;
		}
		return len;
	}
}


// Event mangaer
event_linked_list_t* init_event_linked_list() {
	event_linked_list_t* list = (event_linked_list_t*) 
		malloc(sizeof(event_linked_list_t));
	list->head = (event_node_t*) 
		malloc(sizeof(event_node_t));
	list->head->next = NULL;
	list->head->prev = NULL;
	list->head->event = NULL;
	list->tail = list->head;
	return list;
}
void insert_event_node_to_tail(event_linked_list_t* list, thread_event_t* event) {
	list->tail->next = (event_node_t*) malloc(sizeof(event_node_t));
	list->tail->next->prev = list->tail;
	list->tail = list->tail->next;
	list->tail->event = event;
	list->tail->next = NULL;
}

event_node_t* create_thread_event(event_linked_list_t* list) {
	list->tail->next = (event_node_t*) malloc(sizeof(event_node_t));
	list->tail->next->prev = list->tail;
	list->tail = list->tail->next;
	list->tail->event = (thread_event_t*) malloc(sizeof(thread_event_t));
	list->tail->next = NULL;
	return list->tail;
}

void remove_event_node_from_tail(event_linked_list_t* list) {
	if (is_event_linkded_list_empty(list)) return;
	list->tail = list->tail->prev;
	free(list->tail->next);
	list->tail->next = NULL;
}

int is_event_linkded_list_empty(event_linked_list_t* list) {
	return list->head == list->tail;
}