#include "ttprocesses.h"

int subprocess_amount;
uint64_t subprocess_ids[MAX_SUBPROCESS];

int is_subprocess(uint64_t pid) {
	int i;
	for (i = 0; i < subprocess_amount; i++) 
		if (pid == subprocess_ids[i])
			return 1;
	return 0;
}
inline void push_subprocess(uint64_t pid) {
	subprocess_ids[subprocess_amount++] = pid;
}