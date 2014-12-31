#include <stdio.h>
#include <stdlib.h>
#include "ttevent.h"

#include "trace.inc"

void onexit (void) {
	puts ("Exit");
}

#define MAX_LINE 2048

void process(const char* buf) {
	struct thread_event event;
	parse_event(buf, &event);
}

int main() {
	char buf[MAX_LINE];
	FILE *fd_trace_pipe;
	
	atexit (onexit);
	//if ((fd_trace_pipe = fopen(tracing_file("trace_pipe"), "r")) == NULL) {
	if ((fd_trace_pipe = fopen("/home/hjq/playground/mutex/documents/test-1/filter_trace.log", "r")) == NULL) {
	//if ((fd_trace_pipe = fopen("/home/hjq/playground/mutex/trace.log", "r")) == NULL) {
		perror("fail to read sample_pipe");
		exit(1);
	}

	while (1) {
		if (fgets(buf, MAX_LINE, fd_trace_pipe)) {
			process(buf);
		}
	}
	return 0;
}
