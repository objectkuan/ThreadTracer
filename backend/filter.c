#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include "ttparser.h"
#include "ttlogger.h"

//#include "trace.inc"

void onexit (void) {
	puts ("Exit");
}

int signal_finished_not_come;
void signal_handler(int signo) {
	if (signo == SIGUSR1) {
		print_all_subprocesses(stdout);
		print_all_event_lists(stdout);
		fflush(stdout);
		signal_finished_not_come = 0;
	}
	return;
}

#define MAX_LINE 2048

void only_print(const char* buf) {
	printf("%s", buf);
	fflush(stdout);
}

typedef void (*handler_t)(const char* buf);

/**
 * ./filter pidfile mode infile
 * pidfile:
 *    A file specifying which pids should be traced
 * mode: 
 *    The filtering mode to apply
 *       - Without PID filtered
 *         0 0x0000 only print
 *       - With PID filtered
 *         1 0x0001 only print
 *         2 0x0010 print event, for each trace log line
 *         4 0x0100 print subprocesses, for each trace log line
 *         8 0x1000 print grouped events, for each trace log line
 * infile:
 *    A file caontaining a stream of events, like the trace_pipe
 * outdir:
 *    A directory to hold the output files
 * period:
 *    A period of time in milliseconds that each output file covers
 */
int main(int argc, char** args) {
	char buf[MAX_LINE];
	FILE *fd_trace_pipe;
	handler_t handle;

	FILE *fd_pid_file;
	const char* pid_file;// = args[1];
	char* pid_line = NULL;
	size_t pid_line_len;
	int pid_amount = 0;
	uint64_t pids[100];

	int mode;// = atoi(args[2]);
	char* infile;// = args[3];
	char* outdir;// = args[4];
	long period;// = atol(args[5]);
	uint64_t used_period = period;
	used_period *= 1000;

	// Argument hints
	if (argc != 6) {
		printf("\n\
  pidfile:\n\
     A file specifying which pids should be traced\n\
  mode: \n\
     The filtering mode to apply\n\
        - Without PID filtered\n\
          0 0x0000 only print\n\
        - With PID filtered\n\
          1 0x0001 only print\n\
          2 0x0010 print event, for each trace log line\n\
          4 0x0100 print subprocesses, for each trace log line\n\
          8 0x1000 print grouped events, for each trace log line\n\
  infile:\n\
     A file caontaining a stream of events, like the trace_pipe\n\
  outdir:\n\
     A directory to hold the output files\n\
  period:\n\
     A period of time in milliseconds that each output file covers\n\n");
		return 0;
	}

	// Register a signal for 
	if (signal(SIGUSR1, signal_handler) == SIG_ERR) {
		perror("Cannot set handler for SIGUSR1");
		exit(1);
	}

	pid_file = args[1];
	mode = atoi(args[2]);
	infile = args[3];
	outdir = args[4];
	period = atol(args[5]);


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
	if (mkdir(outdir, S_IRUSR | S_IWUSR | S_IXUSR) == -1) {
		if (errno == EEXIST) {
			fprintf(stdout, "Directory %s already exists and there's no need to create\n", outdir);
		} else {
			fprintf(stderr, "Fail to create output directory %s\n", outdir);
			exit(1);
		}
	} else {
		fprintf(stdout, "Directory %s doesn't exist and is created successfully\n", outdir);
	}
	init_parser(pids, pid_amount, mode, outdir, used_period);

	// Say go, let's go
	handle = (mode == 0 ? &only_print : & parse_event);
	signal_finished_not_come = 1;
	while (signal_finished_not_come) {
		if (fgets(buf, MAX_LINE, fd_trace_pipe)) {
			handle(buf);
		} else break;
	}
	return 0;
}
