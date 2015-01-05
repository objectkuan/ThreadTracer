#include <pthread.h>
#include <unistd.h>
#include <syscall.h>
#include <inttypes.h>
#include <time.h>

#include "trace.inc"

#define NTHREAD 3
pthread_t tid[NTHREAD];
pthread_mutex_t lock;

void* tfn(void *arg) {
	long sid = syscall(SYS_gettid);
	unsigned long long i;
	unsigned long long k;
	append_pid_filter(sid);

	pthread_mutex_lock(&lock);

	k = (unsigned)time(NULL);
	printf("%" PRId64 "\n", k);
	sleep(3);
	k = ((unsigned)time(NULL)) - k;
	printf("%" PRId64 "\n", k);
	fflush(stdout);
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

	sleep(5);
	for (i = 0; i < NTHREAD; i++) {
		pthread_join(tid[i], NULL);
	}
	write_ftrace_file("tracing_on", "0");
	
	pthread_mutex_destroy(&lock);

	return 0;
}
