#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

// Tracing Start
#define _STR(x) #x
#define STR(x) _STR(x)
#define MAX_PATH 256

const char *find_debugfs(void)
{
	static char debugfs[MAX_PATH+1];
	static int debugfs_found;
	char type[100];
	FILE *fp;

	if (debugfs_found)
		return debugfs;

	if ((fp = fopen("/proc/mounts","r")) == NULL) {
		perror("/proc/mounts");
		return NULL;
	}

	while (fscanf(fp, "%*s %"
				STR(MAX_PATH)
				"s %99s %*s %*d %*d\n",
				debugfs, type) == 2) {
		if (strcmp(type, "debugfs") == 0)
			break;
	}
	fclose(fp);

	if (strcmp(type, "debugfs") != 0) {
		fprintf(stderr, "debugfs not mounted");
		return NULL;
	}

	strcat(debugfs, "/tracing/");
	debugfs_found = 1;

	return debugfs;
}

const char *tracing_file(const char *file_name)
{
	static char trace_file[MAX_PATH+1];
	snprintf(trace_file, MAX_PATH, "%s/%s", find_debugfs(), file_name);
	return trace_file;
}

void write_ftrace_file(const char* filename, const char* value) {
	int fd = open(tracing_file(filename), O_WRONLY);
	write(fd, value, strlen(value));
	close(fd);
}

void append_pid_filter(long pid) {
	char str[63];
	sprintf(str, "%ld", pid);
	write_ftrace_file("set_ftrace_pid", str);
}
// Tracing End

