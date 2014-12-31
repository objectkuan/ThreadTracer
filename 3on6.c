#include <pthread.h>
#include <unistd.h>
#include <syscall.h>

#include "trace.inc"

#define NTHREAD 3
#define NLOCK 6
#define RUNNING_SEC 300
#define FPRINT(msg, args...) printf("3on6 \t" msg, ## args);

pthread_t tids[NTHREAD];
pthread_mutex_t locks[NLOCK];

int working;
int count = -1;

void* tfn(void *arg) {
	// Trace the current threads
	int order = ++count;
	long sid = syscall(SYS_gettid);
	append_pid_filter(sid);

	// Debugging
	FPRINT("Thread %d: id is %ld\n", order, sid);
	
	FPRINT("Thread %d: starts.\n", order);
	while (working) {
		int lock_id = rand() % NLOCK;
		int sleeptime[2] = { (rand() % 5), (rand() % 10 > 8 ? 10 : (rand() % 5))  };
		
		FPRINT("Thread %d: sleeps for %ds.\n", order, sleeptime[0]);
		sleep(sleeptime[0]);
		
		FPRINT("Thread %d: wants to work.\n", order);
		pthread_mutex_lock(&(locks[lock_id]));
		FPRINT("Thread %d: works for %ds.\n", order, sleeptime[1]);
		sleep(sleeptime[1]);
		FPRINT("Thread %d: finishes.\n", order);
		pthread_mutex_unlock(&(locks[lock_id]));
	}
	FPRINT("Thread %d: ends.\n", order);
	
	return NULL;
}

int main(void) {
	int i = 0;
	int err;
	
	// Initialize NLOCK locks
	for (i = 0; i < NLOCK; i++) {
		if (pthread_mutex_init(&(locks[i]), NULL) != 0) {
			printf("Mutex init failed\n");
			return 1;
		}
	}
	working = 1;
	
	// Get prepared and start the trace
	append_pid_filter((long) getpid());
	write_ftrace_file("tracing_cpumask", "1");
	write_ftrace_file("tracing_on", "1");
	write_ftrace_file("trace", "0");
	sleep(1);

	// Create the NTHREAD threads
	srand(10086);
	for (i = 0; i < NTHREAD; i++) {
		err = pthread_create(&(tids[i]), NULL, &tfn, NULL);
		if (err != 0)
			printf("Can't create thread :[%s]", strerror(err));
		sleep(1);
	}

	// Wait for threads to finish
	sleep(RUNNING_SEC);
	working = 0;
	for (i = 0; i < NTHREAD; i++)
		pthread_join(tids[i], NULL);

	// Stop the trace
	write_ftrace_file("tracing_on", "0");
	
	// Destroy the locks
	for (i = 0; i < NLOCK; i++)
		pthread_mutex_destroy(&(locks[i]));

	return 0;
}
