CC=gcc
OUTDIR=bin

all: 
	$(CC) -I./include -g -pthread filter.c modules/ttevent.c modules/ttthread_state.c modules/ttdepsolver.c modules/ttparser.c -o $(OUTDIR)/filter
	$(CC) -I./include -g -pthread samples/tmut.c -o $(OUTDIR)/samples/tmut
	$(CC) -I./include -g -pthread samples/3on6.c -o $(OUTDIR)/samples/3on6
	$(CC) -I./include -g -pthread samples/deadlock.c -o $(OUTDIR)/samples/deadlock
