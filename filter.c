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
 * ./filter pid mode infile
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

	int pid = atoi(args[1]);
	int mode = atoi(args[2]);
	char* infile = args[3];

	printf("PID: %d\n", pid);
	printf("Mode: %d\n", mode);
	
	atexit (onexit);
	if ((fd_trace_pipe = fopen(infile, "r")) == NULL) {
		fprintf(stderr, "Fail to read %s\n", infile);
		exit(1);
	}

	init_parser(pid, mode);
	handle = (mode == 0 ? &only_print : & parse_event);
	while (1) {
		if (fgets(buf, MAX_LINE, fd_trace_pipe)) {
				handle(buf);
		} else {
			//
			dump_all(1);
			break;
		}
	}
	return 0;
}
