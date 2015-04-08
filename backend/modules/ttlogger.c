#include "ttlogger.h"

inline void print_raw_line(FILE* fp, const char* buf) {
	fprintf(fp, "%s", buf);
	fflush(fp);
}

inline void print_event(FILE* fp, thread_event_t* event) {
	char event_str[256];
	int len = convert_thread_event_string(event, event_str);
	fprintf(fp, "\t\t%s\n", event_str);
	fflush(fp);
}

extern uint64_t subprocess_amount;
extern uint64_t subprocess_ids[MAX_SUBPROCESS];
inline void print_all_subprocesses(FILE* fp) {
	int i;
	fprintf(fp, "========================\n");
	fprintf(fp, "Subprocesses\n");
	fprintf(fp, "------------------------\n");
	for (i = 0; i < subprocess_amount; i++) {
		fprintf(fp, "%" PRId64 "\t\t", subprocess_ids[i]);
		if (i % 5 == 4) fprintf(fp, "\n");
	}
	if (i % 5) fprintf(fp, "\n");
	fprintf(fp, "========================\n");
	fflush(fp);
}

extern int thread_event_list_len;
extern int futex_event_list_len;
extern int poll_event_list_len;
extern thread_event_list_t thread_event_lists[];
extern futex_event_list_t futex_event_lists[];
extern poll_event_list_t poll_event_lists[];
inline void print_all_event_lists(FILE* fp) {
	int i;
	event_node_t* node;
	fprintf(fp, "===============================\n");
	fprintf(fp, "Thread Events (%d):\n", thread_event_list_len);
	fprintf(fp, "-------------------------------\n");
	for (i = 0; i < thread_event_list_len; i++) {
		thread_event_list_t* list = &(thread_event_lists[i]);
		fprintf(fp, "\tThread %" PRId64 " (%d %s)\n", list->thread_id, list->amount, find_thread(list->thread_id)->thread_name);
		for (node = list->list->head->next; node; node = node->next) {
			print_event(fp, node->event);
		}
	}
	fprintf(fp, "-------------------------------\n");
	fprintf(fp, "Futex Events (%d):\n", futex_event_list_len);
	fprintf(fp, "-------------------------------\n");
	for (i = 0; i < futex_event_list_len; i++) {
		futex_event_list_t* list = &(futex_event_lists[i]);
		fprintf(fp, "\tFutex %" PRId64 " (%d)\n", list->futex, list->amount);
		for (node = list->list->head->next; node; node = node->next) {
			print_event(fp, node->event);
		}
	}
	fprintf(fp, "-------------------------------\n");
	fprintf(fp, "Poll Events (%d):\n", poll_event_list_len);
	fprintf(fp, "-------------------------------\n");
	for (i = 0; i < poll_event_list_len; i++) {
		poll_event_list_t* list = &(poll_event_lists[i]);
		fprintf(fp, "\tPoll fd %" PRId64 " (%d)\n", list->pollfd, list->amount);
		for (node = list->list->head->next; node; node = node->next) {
			print_event(fp, node->event);
		}
	}
	fprintf(fp, "=============================\n");
	fflush(fp);
}


#if 0 
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


inline void print_subprocesses() {
	int i;
	if (run_mode & MODE_MASK_SUBPROCESS) {
		printf("========================\n");
		printf("Subprocesses\n");
		printf("------------------------\n");
		for (i = 0; i < subprocess_amount; i++) {
			printf("%" PRId64 "\t\t", subprocess_ids[i]);
			if (i % 5 == 4) printf("\n");
		}
		if (i % 5) printf("\n");
		printf("========================\n");
		fflush(stdout);
	}
}

inline void print_all_event_lists() {
	if (run_mode & MODE_MASK_EVENT_STAT) {
		dump_all_event_lists();
	}

}

void dump_all(int force) {
	int tmp_run_mode = run_mode;
	if (force) run_mode = 0xf;
	print_all_event_lists();
	print_subprocesses();
	run_mode = tmp_run_mode;
}


void print_thread_event(thread_event_t* event, FILE* fp) {
	switch(event->type) {
		case THREAD_CREATE:
		{
			fprintf(fp, "\t\t%" PRId64 " CREATE_WAKEUP %" PRId64 " %" PRId64 "\n", 
				event->event.thread_create.timestamp, 
				event->event.thread_create.parent_thread_id, 
				event->event.thread_create.child_thread_id
			);
			break;
		}
		case THREAD_ENTER_FUTEX:
		{
			fprintf(fp, "\t\t%" PRId64 " ENTER_FUTEX %" PRId64 " %" PRId64 "\n", 
				event->event.thread_enter_futex.timestamp, 
				event->event.thread_enter_futex.thread_id, 
				event->event.thread_enter_futex.resource_id
			);
			break;
		}
		case THREAD_EXIT_FUTEX:
		{
			fprintf(fp, "\t\t%" PRId64 " EXIT_FUTEX %" PRId64 " %" PRId64 " SLT: %" PRId64 "\n", 
				event->event.thread_exit_futex.timestamp, 
				event->event.thread_exit_futex.thread_id,
				event->event.thread_exit_futex.retval,
				event->event.thread_exit_futex.sleep_time
			);
			break;
		}
		case THREAD_ENTER_POLL:
		{
			fprintf(fp, "\t\t%" PRId64 " ENTER_POLL %" PRId64 " %" PRId64 "\n", 
				event->event.thread_enter_poll.timestamp, 
				event->event.thread_enter_poll.thread_id, 
				event->event.thread_enter_poll.resource_id
			);
			break;
		}
		case THREAD_EXIT_POLL:
		{
			fprintf(fp, "\t\t%" PRId64 " EXIT_POLL %" PRId64 " SLT: %" PRId64 "\n", 
				event->event.thread_exit_poll.timestamp, 
				event->event.thread_exit_poll.thread_id,
				event->event.thread_exit_poll.sleep_time
			);
			break;
		}
		case THREAD_SLEEP:
		{
			fprintf(fp, "\t\t%" PRId64 " SLEEP %" PRId64 "\n", 
				event->event.thread_sleep.timestamp, 
				event->event.thread_sleep.thread_id
			);
			break;
		}
		case THREAD_WAKEUP:
		{
			fprintf(fp, "\t\t%" PRId64 " WAKEUP %" PRId64 " ==> %" PRId64 "\n", 
				event->event.thread_wakeup.timestamp, 
				event->event.thread_wakeup.from_thread_id, 
				event->event.thread_wakeup.to_thread_id
			);
			break;
		}
		case THREAD_EXIT:
		{
			fprintf(fp, "\t\t%" PRId64 " EXIT %" PRId64 "\n", 
				event->event.thread_exit.timestamp, 
				event->event.thread_exit.thread_id);
			break;
		}
	}
	fflush(fp);
}

void print_thread_event_to_stdout(thread_event_t* event) {
	print_thread_event(event, stdout);
}



inline void dump_event_linked_list(event_linked_list_t* list) {
	event_node_t* node;
	printf("dump\n");
	for (node = list->head; node; node = node->next) {
		printf("%p (%p, %p) \n", node, node->prev, node->next);
	}
	fflush(stdout);
}

void dump_all_event_lists() {
	int i;
	event_node_t* node;
	printf("===============================\n");
	printf("Thread Events (%d):\n", thread_event_list_len);
	printf("-------------------------------\n");
	for (i = 0; i < thread_event_list_len; i++) {
		thread_event_list_t* list = &(thread_event_lists[i]);
		printf("\tThread %" PRId64 " (%d %s)\n", list->thread_id, list->amount, find_thread(list->thread_id)->thread_name);
		for (node = list->list->head->next; node; node = node->next) {
			print_thread_event_to_stdout(node->event);
		}
	}
	printf("-------------------------------\n");
	printf("Futex Events (%d):\n", futex_event_list_len);
	printf("-------------------------------\n");
	for (i = 0; i < futex_event_list_len; i++) {
		futex_event_list_t* list = &(futex_event_lists[i]);
		printf("\tFutex %" PRId64 " (%d)\n", list->futex, list->amount);
		for (node = list->list->head->next; node; node = node->next) {
			print_thread_event_to_stdout(node->event);
		}
	}
	printf("-------------------------------\n");
	printf("Poll Events (%d):\n", poll_event_list_len);
	printf("-------------------------------\n");
	for (i = 0; i < poll_event_list_len; i++) {
		poll_event_list_t* list = &(poll_event_lists[i]);
		printf("\tPoll fd %" PRId64 " (%d)\n", list->pollfd, list->amount);
		for (node = list->list->head->next; node; node = node->next) {
			print_thread_event_to_stdout(node->event);
		}
	}
	printf("=============================\n");
	fflush(stdout);
}
#endif