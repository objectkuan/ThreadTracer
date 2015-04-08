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

// Printing subprocesses
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

// Printing thread event lists
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

const char* frontend_output_dir = NULL;
uint64_t used_period = 0;
void print_frontend_event(uint64_t timestamp, const char* fe_event) {
	int times_5s = timestamp / used_period;
	if (times_5s > last_times_5s) {
		char new_file_path[1024];

		if (curr_frontend_file && last_period > 0) 
			fclose(curr_frontend_file);
		
		sprintf(new_file_path, "%s/%u.log", frontend_output_dir, last_period);
		printf("Ftrace parsing now is output to %s\n", new_file_path);
		fflush(stdout);
		if (!(curr_frontend_file = fopen(new_file_path, "w"))) {
			fprintf(stderr, "Error while opening file %s\n", new_file_path);
			fflush(stderr);
		}
		
		last_times_5s = times_5s;
		last_period++;
	}
	fprintf(curr_frontend_file, "%s", fe_event);
	fflush(curr_frontend_file);
}
