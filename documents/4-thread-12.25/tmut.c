#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <syscall.h>

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

#define NTHREAD 3
pthread_t tid[NTHREAD];
pthread_mutex_t lock;

void* tfn(void *arg) {
	long sid = syscall(SYS_gettid);
	append_pid_filter(sid);

	pthread_mutex_lock(&lock);
	sleep(4);
	pthread_mutex_unlock(&lock);
	return NULL;
}

int main(void) {
	int i = 0;
	int err;

	if (pthread_mutex_init(&lock, NULL) != 0) {
		printf("Mutex init failed\n");
		return 1;
	}
	
	append_pid_filter((long) getpid());
	write_ftrace_file("tracing_cpumask", "1");
	write_ftrace_file("tracing_on", "1");
	write_ftrace_file("trace", "0");
	sleep(1);

	for (i = 0; i < NTHREAD; i++) {
		err = pthread_create(&(tid[i]), NULL, &tfn, NULL);
		if (err != 0)
			printf("Can't create thread :[%s]", strerror(err));
		sleep(1);
	}

	sleep(10);
	write_ftrace_file("tracing_on", "0");
	for (i = 0; i < NTHREAD; i++) {
		pthread_join(tid[i], NULL);
	}
	
	pthread_mutex_destroy(&lock);

	return 0;
}
