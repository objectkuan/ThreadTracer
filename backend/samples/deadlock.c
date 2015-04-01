#include <pthread.h>
#include <unistd.h>
#include <syscall.h>

#include "trace.inc"

pthread_t tid[2];
pthread_mutex_t lock[2];

void* tfn1(void *arg) {
	long sid = syscall(SYS_gettid);
	append_pid_filter(sid);

	printf("Thread 1 wants Mutex 0\n");
	pthread_mutex_lock(&lock[0]);
	printf("Thread 1 gets Mutex 0\n");
	sleep(2);
	printf("Thread 1 wants Mutex 1\n");
	pthread_mutex_lock(&lock[1]);
	printf("Thread 1 gets Mutex 1\n");
	sleep(2);
	printf("Thread 1 discards Mutex 1\n");
	pthread_mutex_unlock(&lock[1]);
	printf("Thread 1 looses Mutex 1\n");
	printf("Thread 1 discards Mutex 0\n");
	pthread_mutex_unlock(&lock[0]);
	printf("Thread 1 looses Mutex 0\n");

	return NULL;
}
void* tfn2(void *arg) {
	long sid = syscall(SYS_gettid);
	append_pid_filter(sid);

	printf("Thread 2 wants Mutex 1\n");
	pthread_mutex_lock(&lock[1]);
	printf("Thread 2 gets Mutex 1\n");
	sleep(2);
	if (pthread_mutex_trylock(&lock[0])) {
		printf("shit\n");
	}
	printf("Thread 2 wants Mutex 0\n");
	pthread_mutex_lock(&lock[0]);
	printf("Thread 2 gets Mutex 0\n");
	sleep(2);
	printf("Thread 2 discards Mutex 0\n");
	pthread_mutex_unlock(&lock[0]);
	printf("Thread 2 looses Mutex 0\n");
	printf("Thread 2 discards Mutex 1\n");
	pthread_mutex_unlock(&lock[1]);
	printf("Thread 2 looses Mutex 1\n");

	return NULL;
}

int main(void) {
	int i = 0;
	int err;

	for (i = 0; i < 2; i++)
		if (pthread_mutex_init(&lock[i], NULL) != 0) {
			printf("Mutex init failed\n");
			return 1;
		}
	
	append_pid_filter((long) getpid());
	write_ftrace_file("tracing_cpumask", "1");
	write_ftrace_file("tracing_on", "1");
	write_ftrace_file("trace", "0");
	sleep(1);

	err = pthread_create(&(tid[0]), NULL, &tfn1, NULL);
	if (err != 0)
		printf("Can't create thread :[%s]", strerror(err));
	sleep(1);
	err = pthread_create(&(tid[1]), NULL, &tfn2, NULL);
	if (err != 0)
		printf("Can't create thread :[%s]", strerror(err));
	sleep(1);

	write_ftrace_file("tracing_on", "0");
	pthread_join(tid[0], NULL);
	pthread_join(tid[1], NULL);
	
	pthread_mutex_destroy(&lock[0]);
	pthread_mutex_destroy(&lock[1]);

	return 0;
}
