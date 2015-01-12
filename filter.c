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
}

typedef void (*handler_t)(const char* buf);

/**
 * ./filter pid mode infile
 * mode: 
 *    0 only print
 *    1 only print, with pid filtered
 *    2 print event for each trace log line
 *    3 print subprocesses
 *    4 print grouped events
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
