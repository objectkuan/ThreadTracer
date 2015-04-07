#include <stdio.h>
#include <stdlib.h>
#include "ttparser.h"
#include <unistd.h>

//#include "trace.inc"

void onexit (void) {
	puts ("Exit");
}

#define MAX_LINE 2048


void only_print(const char* buf) {
	printf("%s", buf);
	fflush(stdout);
}

typedef void (*handler_t)(const char* buf);

/**
 * ./filter pidfile mode infile
 * mode:
 *    Without PID filtered
 *    0 only print
 *    With PID filtered
 *    1 only print
 *    2 print event, for each trace log line
 *    4 print subprocesses, for each trace log line
 *    8 print grouped events, for each trace log line
 */
int main(int argc, char** args) {
	char buf[MAX_LINE];
	FILE *fd_trace_pipe;
	handler_t handle;

	FILE *fd_pid_file;
	const char* pid_file = args[1];
	char* pid_line = NULL;
	size_t pid_line_len;
	int pid_amount = 0;
	uint64_t pids[100];

	int mode = atoi(args[2]);
	char* infile = args[3];

	printf("mode: %d\n", mode);
	printf("pid in files: %s\n", pid_file);
	
	atexit (onexit);

	// Open trace file
	if ((fd_trace_pipe = fopen(infile, "r")) == NULL) {
		fprintf(stderr, "Fail to read trace file %s\n", infile);
		exit(1);
	}

	// Read pids
	printf("[");
	if ((fd_pid_file = fopen(pid_file, "r")) == NULL) {
		fprintf(stderr, "Fail to read pid file %s\n", pid_file);
		exit(1);
	}
	while (getline(&pid_line, &pid_line_len, fd_pid_file) != -1) {
		pids[pid_amount] = atoi(pid_line);
		if (pids[pid_amount]) {
			printf("%d,", pids[pid_amount]);
			pid_amount++;
		}
	}
	printf("0]\n");
	fclose(fd_pid_file);
	if (pid_line) free(pid_line);
	
	// Initialize parser
	init_parser(pids, pid_amount, mode);

	// Say go, let's go
	handle = (mode == 0 ? &only_print : & parse_event);
	while (1) {
		if (fgets(buf, MAX_LINE, fd_trace_pipe)) {
				handle(buf);
		} else {
			//dump_all(1);
			break;
		}
	}
	return 0;
}
