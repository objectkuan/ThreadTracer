#include "ttevent.h"

void print_thread_event(thread_event_t* event) {
	switch(event->type) {
		case THREAD_CREATE:
		{
			printf("\t\t%" PRId64 " CREATE_WAKEUP %" PRId64 " %" PRId64 "\n", 
				event->event.thread_create.timestamp, 
				event->event.thread_create.parent_thread_id, 
				event->event.thread_create.child_thread_id
			);
			break;
		}
		case THREAD_WAIT_FUTEX:
		{
			printf("\t\t%" PRId64 " WAIT_FUTEX %" PRId64 " %" PRId64 "\n", 
				event->event.thread_wait_futex.timestamp, 
				event->event.thread_wait_futex.thread_id, 
				event->event.thread_wait_futex.resource_id
			);
			break;
		}
		case THREAD_GET_FUTEX:
		{
			printf("\t\t%" PRId64 " GET_FUTEX %" PRId64 "\n", 
				event->event.thread_get_futex.timestamp, 
				event->event.thread_get_futex.thread_id
			);
			break;
		}
		case THREAD_RELEASE_FUTEX:
		{
			printf("\t\t%" PRId64 " RELEASE_FUTEX %" PRId64 " %" PRId64 "\n", 
				event->event.thread_release_futex.timestamp, 
				event->event.thread_release_futex.thread_id, 
				event->event.thread_release_futex.resource_id
			);
			break;
		}
		case THREAD_SLEEP:
		{
			printf("\t\t%" PRId64 " SLEEP %" PRId64 "\n", 
				event->event.thread_sleep.timestamp, 
				event->event.thread_sleep.thread_id
			);
			break;
		}
		case THREAD_WAKEUP:
		{
			printf("\t\t%" PRId64 " WAKEUP %" PRId64 " ==>%" PRId64 "\n", 
				event->event.thread_wakeup.timestamp, 
				event->event.thread_wakeup.from_thread_id, 
				event->event.thread_wakeup.to_thread_id
			);
			break;
		}
		case THREAD_EXIT:
		{
			printf("\t\t%" PRId64 " EXIT %" PRId64 "\n", 
				event->event.thread_exit.timestamp, 
				event->event.thread_exit.thread_id);
			break;
		}
	}
	fflush(stdout);
}


// Event mangaer
event_linked_list_t* init_event_linked_list() {
	event_linked_list_t* list = (event_linked_list_t*) 
		malloc(sizeof(event_linked_list_t));
	list->head = (event_node_t*) 
		malloc(sizeof(event_node_t));
	list->head->next = NULL;
	list->head->event = NULL;
	list->tail = list->head;
	list->length = 0;
	return list;
}
void insert_event_node_to_tail(event_linked_list_t* list, thread_event_t* event) {
	list->tail->next = (event_node_t*) malloc(sizeof(event_node_t));
	list->tail = list->tail->next;
	list->tail->event = event;
	list->tail->next = NULL;
}

event_node_t* create_thread_event(event_linked_list_t* list) {
	list->tail->next = (event_node_t*) malloc(sizeof(event_node_t));
	list->tail = list->tail->next;
	list->tail->event = (thread_event_t*) malloc(sizeof(thread_event_t));
	list->tail->next = NULL;
	return list->tail;
}
